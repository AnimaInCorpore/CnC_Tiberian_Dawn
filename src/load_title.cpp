/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "legacy/type.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/wwlib32.h"
#include "legacy/mixfile.h"
#include "legacy/ccfile.h"
#include "legacy/cdfile.h"
#include "legacy/gscreen.h"

#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace {

constexpr int kPaletteSize = 256 * 3;

#pragma pack(push, 1)
struct PcxHeader {
	unsigned char manufacturer;
	unsigned char version;
	unsigned char encoding;
	unsigned char bits_per_pixel;
	uint16_t x_min;
	uint16_t y_min;
	uint16_t x_max;
	uint16_t y_max;
	uint16_t h_dpi;
	uint16_t v_dpi;
	unsigned char palette16[48];
	unsigned char reserved;
	unsigned char planes;
	uint16_t bytes_per_line;
	uint16_t palette_type;
	uint16_t h_screen_size;
	uint16_t v_screen_size;
	unsigned char filler[54];
};
#pragma pack(pop)

struct DecodedPcx {
	int width = 0;
	int height = 0;
	std::vector<unsigned char> pixels;
	unsigned char palette[kPaletteSize]{};
	bool has_palette = false;
};

#pragma pack(push, 1)
struct CpsHeader {
	std::uint16_t file_size = 0;
	std::uint16_t compression = 0;
	std::uint32_t uncompressed_size = 0;
	std::uint16_t palette_size = 0;
};

struct MixFileHeader {
	std::uint16_t count = 0;
	std::uint32_t size = 0;
};
#pragma pack(pop)

static_assert(sizeof(MixFileHeader) == 6, "Mix header size mismatch");

void Apply_Title_Palette(const unsigned char* source, unsigned char* dest) {
	if (!source) return;

	unsigned char* target = dest ? dest : Palette;
	if (target) {
		std::memcpy(target, source, kPaletteSize);
	}

	if (!GamePalette) {
		GamePalette = new unsigned char[kPaletteSize];
	}
	if (GamePalette) {
		std::memcpy(GamePalette, source, kPaletteSize);
	}
}

void Present_Title(GraphicViewPortClass* view) {
	if (view == &HidPage) {
		GScreenClass::Blit_Display();
	}
}

bool Lcw_Decompress(const unsigned char* src, std::size_t src_size, std::vector<unsigned char>& out,
                    std::size_t expected_size) {
	if (!src) return false;

	std::vector<unsigned char> result;
	result.resize(expected_size);

	std::size_t out_pos = 0;
	std::size_t pos = 0;

	// If the stream starts with 0, offsets are relative instead of absolute.
	const bool relative = src_size > 0 && src[0] == 0;
	if (relative) {
		pos = 1;
	}

	auto ensure_space = [&](std::size_t count) { return out_pos + count <= result.size(); };

	while (pos < src_size && out_pos < result.size()) {
		const unsigned char code = src[pos++];

		// Command 1: literal copy.
		if ((code & 0xC0) == 0x80) {
			const std::size_t count = static_cast<std::size_t>(code & 0x3F);
			if (count == 0) break;  // Terminator (0x80).
			if (pos + count > src_size || !ensure_space(count)) return false;
			std::memcpy(result.data() + out_pos, src + pos, count);
			out_pos += count;
			pos += count;
			continue;
		}

		// Command 2: short relative copy.
		if ((code & 0x80) == 0) {
			const std::size_t count = static_cast<std::size_t>((code >> 4) & 0x07) + 3;
			if (pos >= src_size) return false;
			const std::size_t offset = static_cast<std::size_t>((code & 0x0F) << 8) + src[pos++];
			if (offset == 0 || offset > out_pos) return false;
			const std::size_t from = out_pos - offset;
			if (!ensure_space(count) || from + count > result.size()) return false;
			std::memmove(result.data() + out_pos, result.data() + from, count);
			out_pos += count;
			continue;
		}

		// Commands 3/4/5: longer copies or fills.
		if (code == 0xFE) {
			if (pos + 3 > src_size) return false;
			const std::size_t count = static_cast<std::size_t>(src[pos]) | (static_cast<std::size_t>(src[pos + 1]) << 8);
			const unsigned char value = src[pos + 2];
			pos += 3;
			if (!ensure_space(count)) return false;
			std::memset(result.data() + out_pos, value, count);
			out_pos += count;
			continue;
		}

		if (code == 0xFF) {
			if (pos + 4 > src_size) return false;
			const std::size_t count = static_cast<std::size_t>(src[pos]) |
			                          (static_cast<std::size_t>(src[pos + 1]) << 8);
			const std::size_t offset = static_cast<std::size_t>(src[pos + 2]) |
			                           (static_cast<std::size_t>(src[pos + 3]) << 8);
			pos += 4;
			if (relative && offset > out_pos) return false;
			const std::size_t from = relative ? out_pos - offset : offset;
			if (!ensure_space(count) || from + count > result.size()) return false;
			std::memmove(result.data() + out_pos, result.data() + from, count);
			out_pos += count;
			continue;
		}

		// Command 3: medium copy.
		const std::size_t count = static_cast<std::size_t>(code & 0x3F) + 3;
		if (pos + 2 > src_size) return false;
		const std::size_t offset = static_cast<std::size_t>(src[pos]) |
		                           (static_cast<std::size_t>(src[pos + 1]) << 8);
		pos += 2;
		if (relative && offset > out_pos) return false;
		const std::size_t from = relative ? out_pos - offset : offset;
		if (!ensure_space(count) || from + count > result.size()) return false;
		std::memmove(result.data() + out_pos, result.data() + from, count);
		out_pos += count;
	}

	if (out_pos != result.size()) {
		return false;
	}

	out.swap(result);
	return true;
}

bool Decode_Cps(const std::vector<unsigned char>& data, DecodedPcx& output) {
	if (data.size() < sizeof(CpsHeader)) return false;

	CpsHeader header{};
	std::memcpy(&header, data.data(), sizeof(header));

	if (header.uncompressed_size == 0) return false;

	const std::size_t palette_offset = sizeof(header);
	const std::size_t data_offset = palette_offset + header.palette_size;
	if (data_offset > data.size()) return false;

	const unsigned char* payload = data.data() + data_offset;
	const std::size_t payload_size = data.size() - data_offset;

	std::vector<unsigned char> pixels;
	switch (header.compression) {
		case 0:
			pixels.assign(payload, payload + payload_size);
			break;
		case 4:
			if (!Lcw_Decompress(payload, payload_size, pixels, header.uncompressed_size)) {
				return false;
			}
			break;
		default:
			return false;
	}

	if (pixels.size() != header.uncompressed_size) return false;

	output.has_palette = header.palette_size >= kPaletteSize;
	if (output.has_palette) {
		std::memcpy(output.palette, data.data() + palette_offset, kPaletteSize);
	}

	// Infer dimensions (C&C CPS screens are usually 320x200; prefer 640-wide only when it yields
	// a plausible full-screen aspect such as 640x400 or 640x480).
	auto height_for_width = [&](int candidate_width) -> int {
		if (candidate_width <= 0) return 0;
		if (header.uncompressed_size % static_cast<std::uint32_t>(candidate_width) != 0) return 0;
		const std::size_t h = header.uncompressed_size / static_cast<std::uint32_t>(candidate_width);
		return h > static_cast<std::size_t>(std::numeric_limits<int>::max()) ? 0
		                                                                      : static_cast<int>(h);
	};

	const int height_320 = height_for_width(320);
	const int height_640 = height_for_width(640);

	auto aspect_ok = [](int w, int h) {
		if (w <= 0 || h <= 0) return false;
		const float aspect = static_cast<float>(w) / static_cast<float>(h);
		return aspect >= 1.2f && aspect <= 1.8f;
	};

	int width = 320;
	int height = height_320;
	if (height_640 > 0 && aspect_ok(640, height_640)) {
		width = 640;
		height = height_640;
	} else if (height <= 0 && height_640 > 0) {
		// Fall back to any viable 640-wide guess if the 320 path fails divisibility.
		width = 640;
		height = height_640;
	}

	if (height <= 0) {
		return false;
	}

	output.width = width;
	output.height = height;
	output.pixels.swap(pixels);
	return true;
}

bool Blit_With_Scale(const DecodedPcx& image, GraphicViewPortClass* view) {
	if (!view) return false;
	GraphicBufferClass* buffer = view->Get_Graphic_Buffer();
	if (!buffer) return false;

	const int target_w = view->Get_Width();
	const int target_h = view->Get_Height();
	if (!buffer->Is_Valid() || buffer->Get_Width() < target_w || buffer->Get_Height() < target_h) {
		buffer->Init(target_w, target_h, nullptr, 0, GBC_NONE);
		view->Configure(buffer, view->Get_XPos(), view->Get_YPos(), target_w, target_h);
	}

	unsigned char* dest = buffer->Get_Buffer();
	if (!dest) return false;

	const int pitch = buffer->Get_Width();
	const float scale_x = static_cast<float>(target_w) / static_cast<float>(std::max(1, image.width));
	const float scale_y = static_cast<float>(target_h) / static_cast<float>(std::max(1, image.height));
	const float scale = std::max(0.01f, std::min(scale_x, scale_y));
	const int copy_w = std::max(1, std::min(target_w, static_cast<int>(image.width * scale + 0.5f)));
	const int copy_h = std::max(1, std::min(target_h, static_cast<int>(image.height * scale + 0.5f)));
	const int offset_x = view->Get_XPos() + std::max(0, (target_w - copy_w) / 2);
	const int offset_y = view->Get_YPos() + std::max(0, (target_h - copy_h) / 2);

	// Clear destination to avoid junk in the padded area.
	std::fill(dest, dest + pitch * buffer->Get_Height(), 0);

	for (int y = 0; y < copy_h; ++y) {
		const int src_y = std::min(image.height - 1, static_cast<int>(static_cast<float>(y) / scale));
		const unsigned char* src_row = image.pixels.data() + src_y * image.width;
		unsigned char* dst_row = dest + (offset_y + y) * pitch + offset_x;
		for (int x = 0; x < copy_w; ++x) {
			const int src_x = std::min(image.width - 1, static_cast<int>(static_cast<float>(x) / scale));
			dst_row[x] = src_row[src_x];
		}
	}

	return true;
}

bool Load_Title_From_Cps_Mix(const char* mix_name, GraphicViewPortClass* video_page,
                             unsigned char* palette) {
	if (!mix_name || !video_page) return false;

	const std::string base_name(mix_name);
	const std::string gdi_path = "CD/GDI/" + base_name;
	const std::string nod_path = "CD/NOD/" + base_name;
	const std::string root_path = "CD/" + base_name;

	std::vector<std::string> search_paths{base_name, gdi_path, nod_path, root_path};
	std::vector<unsigned char> mix_data;

	for (auto const& path : search_paths) {
		std::ifstream file(path, std::ios::binary);
		if (!file) continue;
		file.seekg(0, std::ios::end);
		const std::streamoff size = file.tellg();
		if (size <= 0) continue;
		file.seekg(0, std::ios::beg);
		mix_data.resize(static_cast<std::size_t>(size));
		file.read(reinterpret_cast<char*>(mix_data.data()), size);
		if (file) {
			break;
		}
		mix_data.clear();
	}

	if (mix_data.empty()) {
		CCDebugString("Load_Title_Screen: unable to open ");
		CCDebugString(mix_name);
		CCDebugString(".\n");
		return false;
	}

	if (mix_data.size() < sizeof(MixFileHeader)) {
		return false;
	}

	MixFileHeader mix_header{};
	std::memcpy(&mix_header, mix_data.data(), sizeof(mix_header));
	if (mix_header.count == 0) return false;

	const std::uint32_t entries_bytes =
	    static_cast<std::uint32_t>(mix_header.count * sizeof(MixFileClass::SubBlock));
	const std::uint32_t data_base =
	    static_cast<std::uint32_t>(sizeof(MixFileHeader) + entries_bytes);
	const std::uint32_t data_end = data_base + mix_header.size;
	if (mix_data.size() < data_end) return false;

	std::vector<MixFileClass::SubBlock> entries(mix_header.count);
	std::memcpy(entries.data(), mix_data.data() + sizeof(MixFileHeader), entries_bytes);

	const MixFileClass::SubBlock* best = nullptr;
	CpsHeader best_header{};
	double best_palette_score = -1.0;

	for (auto const& entry : entries) {
		const std::uint32_t start = data_base + entry.Offset;
		const std::uint32_t end = start + entry.Size;
		if (start < data_base || end > data_end || end > mix_data.size() ||
		    entry.Size < sizeof(CpsHeader)) {
			continue;
		}

		CpsHeader header{};
		std::memcpy(&header, mix_data.data() + start, sizeof(header));

		if ((header.compression != 0 && header.compression != 4) || header.uncompressed_size == 0) {
			continue;
		}
		if (header.palette_size < kPaletteSize) {
			continue;
		}

		// Use palette brightness as a tiebreaker so we pick the variant with a non-black
		// background instead of the darkest entry.
		double palette_score = -1.0;
		const std::uint32_t palette_base = start + sizeof(CpsHeader);
		if (palette_base + kPaletteSize <= end) {
			std::uint64_t palette_sum = 0;
			for (std::size_t i = 0; i < kPaletteSize; ++i) {
				palette_sum += static_cast<std::uint64_t>(mix_data[palette_base + i]);
			}
			palette_score = static_cast<double>(palette_sum) / static_cast<double>(kPaletteSize);
		}

		// Prefer larger uncompressed payloads, then larger compressed size.
		if (!best || header.uncompressed_size > best_header.uncompressed_size ||
		    (header.uncompressed_size == best_header.uncompressed_size && palette_score > best_palette_score) ||
		    (header.uncompressed_size == best_header.uncompressed_size && palette_score == best_palette_score &&
		     entry.Size > best->Size)) {
			best = &entry;
			best_header = header;
			best_palette_score = palette_score;
		}
	}

	if (!best) {
		CCDebugString("Load_Title_Screen: no CPS candidates found in ");
		CCDebugString(mix_name);
		CCDebugString(".\n");
		return false;
	}

	const std::uint32_t best_start = data_base + best->Offset;
	std::vector<unsigned char> entry_data(mix_data.begin() + best_start,
	                                      mix_data.begin() + best_start + best->Size);

	DecodedPcx cps{};
	if (!Decode_Cps(entry_data, cps)) {
		CCDebugString("Load_Title_Screen: CPS decode failed for ");
		CCDebugString(mix_name);
		CCDebugString(".\n");
		return false;
	}

	if (cps.has_palette) {
		Apply_Title_Palette(cps.palette, palette);
	}

	CCDebugString("Load_Title_Screen: loaded CPS title from ");
	CCDebugString(mix_name);
	CCDebugString(".\n");
	return Blit_With_Scale(cps, video_page);
}

bool Decode_Pcx_Buffer(const unsigned char* data, std::size_t data_size, DecodedPcx& output) {
	if (!data || data_size < sizeof(PcxHeader)) return false;

	PcxHeader header{};
	std::memcpy(&header, data, sizeof(PcxHeader));

	if (header.manufacturer != 0x0A || header.encoding != 1 || header.bits_per_pixel != 8 || header.planes != 1) {
		return false;
	}

	const int width = static_cast<int>(header.x_max - header.x_min + 1);
	const int height = static_cast<int>(header.y_max - header.y_min + 1);
	if (width <= 0 || height <= 0) {
		return false;
	}

	const std::size_t payload_offset = sizeof(PcxHeader);
	if (data_size <= payload_offset) return false;
	const std::size_t payload_size = data_size - payload_offset;
	const unsigned char* payload = data + payload_offset;

	const std::size_t palette_block_size = 1 + kPaletteSize;
	const std::size_t palette_offset =
	    payload_size > palette_block_size ? payload_size - palette_block_size : payload_size;
	const bool has_palette = payload_size >= palette_block_size && payload[palette_offset] == 0x0C;

	const std::size_t pixel_data_size = has_palette ? palette_offset : payload_size;
	output.width = width;
	output.height = height;
	output.pixels.assign(static_cast<std::size_t>(width) * height, 0);

	std::size_t pos = 0;
	for (int y = 0; y < height; ++y) {
		int x = 0;
		while (x < width && pos < pixel_data_size) {
			unsigned char value = payload[pos++];
			int count = 1;
			if ((value & 0xC0) == 0xC0 && pos < pixel_data_size) {
				count = value & 0x3F;
				value = payload[pos++];
			}
			const int write_count = std::min(count, width - x);
			std::fill_n(output.pixels.data() + (y * width) + x, write_count, value);
			x += count;
		}
	}

	if (has_palette) {
		const unsigned char* palette_data = payload + palette_offset + 1;
		for (int index = 0; index < kPaletteSize; ++index) {
			output.palette[index] = static_cast<unsigned char>(palette_data[index] << 2);
		}
		output.has_palette = true;
	} else {
		output.has_palette = false;
	}

	return true;
}

bool Decode_Pcx(const std::string& path, DecodedPcx& output) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return false;
	}

	file.seekg(0, std::ios::end);
	const std::streamoff file_size = file.tellg();
	if (file_size < static_cast<std::streamoff>(sizeof(PcxHeader))) {
		return false;
	}
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> data(static_cast<std::size_t>(file_size));
	file.read(reinterpret_cast<char*>(data.data()), file_size);
	if (!file) return false;

	return Decode_Pcx_Buffer(data.data(), data.size(), output);
}

void Fill_Fallback(GraphicViewPortClass* view, unsigned char* palette) {
	if (!view) return;
	GraphicBufferClass* buffer = view->Get_Graphic_Buffer();
	if (!buffer) return;
	if (!buffer->Is_Valid()) {
		buffer->Init(view->Get_Width(), view->Get_Height(), nullptr, 0, GBC_NONE);
	}
	unsigned char* dest = buffer->Get_Buffer();
	if (!dest) return;

	const int pitch = buffer->Get_Width();
	const int width = view->Get_Width();
	const int height = view->Get_Height();
	const int origin_x = view->Get_XPos();
	const int origin_y = view->Get_YPos();

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const unsigned char value = static_cast<unsigned char>((x + y) % 256);
			dest[(origin_y + y) * pitch + origin_x + x] = value;
			if (palette && x < 256 && y == 0) {
				const std::size_t offset = static_cast<std::size_t>(x) * 3;
				palette[offset + 0] = static_cast<unsigned char>(value << 2);
				palette[offset + 1] = static_cast<unsigned char>(value << 2);
				palette[offset + 2] = static_cast<unsigned char>(value << 2);
			}
		}
	}
}

}  // namespace

void Load_Title_Screen(char* name, GraphicViewPortClass* video_page, unsigned char* palette) {
	if (!name || !video_page) return;

	DecodedPcx pcx{};
	bool loaded = false;

	static bool mixes_registered = false;
	if (!mixes_registered) {
		// Title art normally lives in GENERAL.MIX/CONQUER.MIX on the Win95 discs.
		static MixFileClass general_mix("GENERAL.MIX");
		static MixFileClass conquer_mix("CONQUER.MIX");
		(void)general_mix;
		(void)conquer_mix;
		mixes_registered = true;
	}

	// Prefer loading from resident MIX archives.
	void* ptr = nullptr;
	MixFileClass* mix = nullptr;
	long offset = 0;
	long size = 0;
	if (MixFileClass::Offset(name, &ptr, &mix, &offset, &size)) {
		if (ptr) {
			loaded = Decode_Pcx_Buffer(static_cast<unsigned char*>(ptr), static_cast<std::size_t>(size), pcx);
		} else if (mix && size > 0) {
			CCFileClass file(mix->Filename);
			if (file.Open()) {
				std::vector<unsigned char> data(static_cast<std::size_t>(size));
				file.Seek(offset, SEEK_SET);
				const long read = file.Read(data.data(), size);
				file.Close();
				if (read == size) {
					loaded = Decode_Pcx_Buffer(data.data(), data.size(), pcx);
				}
			}
		}
	}

	if (!loaded && Decode_Pcx(name, pcx)) {
		loaded = true;
	}

	if (loaded) {
		if (pcx.has_palette) {
			Apply_Title_Palette(pcx.palette, palette ? palette : Palette);
		}
		Blit_With_Scale(pcx, video_page);
		Present_Title(video_page);
		return;
	}

	CCDebugString("Load_Title_Screen: attempting CPS fallback.\n");
	// Legacy high-res title screens are stored as CPS inside the disc mix archives.
	if (Load_Title_From_Cps_Mix("GENERAL.MIX", video_page, palette) ||
	    Load_Title_From_Cps_Mix("CONQUER.MIX", video_page, palette)) {
		Present_Title(video_page);
		return;
	}

	CCDebugString("Load_Title_Screen: failed to open PCX.\n");
	Fill_Fallback(video_page, palette);
	Present_Title(video_page);
}
