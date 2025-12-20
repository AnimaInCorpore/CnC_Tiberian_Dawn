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
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

constexpr int kPaletteSize = 256 * 3;
constexpr std::size_t kMaxScanSize = 2 * 1024 * 1024;

unsigned char Normalize_Pcx_Channel(unsigned char value) {
	// Legacy PCX loader always downshifted palette entries to 6-bit.
	return static_cast<unsigned char>(value >> 2);
}

unsigned char Normalize_Cps_Channel(unsigned char value) {
	// CPS palettes are often already in 6-bit range; only downshift when needed.
	if (value > 63) {
		value = static_cast<unsigned char>(value >> 2);
	}
	return value;
}

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

struct MixSubBlock {
	std::uint32_t crc = 0;
	std::uint32_t offset = 0;
	std::uint32_t size = 0;
};

bool Decode_Cps(const std::vector<unsigned char>& data, DecodedPcx& output);

#pragma pack(push, 1)
struct CpsHeader {
	std::uint16_t file_size = 0;
	std::uint16_t compression = 0;
	std::uint32_t uncompressed_size = 0;
	std::uint16_t palette_size = 0;
};

#pragma pack(pop)

bool Decode_Pcx_Buffer(const unsigned char* data, std::size_t data_size, DecodedPcx& output);

bool Load_File_To_Buffer(const char* name, std::vector<unsigned char>& data) {
	if (!name) return false;

	CCFileClass file(name);
	if (!file.Open()) {
		return false;
	}

	const long length = file.Size();
	if (length <= 0) {
		file.Close();
		return false;
	}

	data.resize(static_cast<std::size_t>(length));
	const long read = file.Read(data.data(), length);
	file.Close();
	return read == length;
}

bool Decode_Pcx_Or_Cps(const std::vector<unsigned char>& data, DecodedPcx& output) {
	if (data.empty()) return false;
	if (Decode_Pcx_Buffer(data.data(), data.size(), output)) {
		return true;
	}
	return Decode_Cps(data, output);
}

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

	const unsigned char* palette_data = data.data() + palette_offset;
	std::size_t palette_bytes = header.palette_size;
	if (palette_bytes >= kPaletteSize + 1 && palette_data[0] == 0x0C) {
		// Some CPS assets store the palette with a PCX-style marker prefix.
		++palette_data;
		--palette_bytes;
	}
	output.has_palette = palette_bytes >= kPaletteSize;
	if (output.has_palette) {
		for (int index = 0; index < kPaletteSize; ++index) {
			output.palette[index] = Normalize_Cps_Channel(palette_data[index]);
		}
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

	if (payload_size < kPaletteSize) return false;
	// Legacy loader always pulled the last 768 bytes as the palette (marker optional).
	const std::size_t palette_offset = payload_size - kPaletteSize;
	const std::size_t pixel_data_size = palette_offset;
	output.width = width;
	output.height = height;
	output.pixels.assign(static_cast<std::size_t>(width) * height, 0);

	// Decode RLE stream sequentially into the pixel buffer. PCX RLE runs may span
	// across scanline boundaries, so treating rows independently can drop pixels
	// or desynchronize the stream. Decode into a flat buffer to preserve runs.
	std::size_t pos = 0;
	const std::size_t total_pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
	std::size_t out_pos = 0;
	while (out_pos < total_pixels && pos < pixel_data_size) {
		unsigned char value = payload[pos++];
		std::size_t count = 1;
		if ((value & 0xC0) == 0xC0) {
			if (pos >= pixel_data_size) break; // malformed stream
			count = static_cast<std::size_t>(value & 0x3F);
			value = payload[pos++];
		}
		const std::size_t write_count = std::min(count, total_pixels - out_pos);
		std::fill_n(output.pixels.data() + out_pos, write_count, value);
		out_pos += write_count;
	}

	output.has_palette = true;
	const unsigned char* palette_data = payload + palette_offset;
	for (int index = 0; index < kPaletteSize; ++index) {
		output.palette[index] = Normalize_Pcx_Channel(palette_data[index]);
	}

	return true;
}

[[maybe_unused]] bool Decode_Pcx(const std::string& path, DecodedPcx& output) {
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

std::vector<std::string> Discover_Mix_Files(const char* cd_subfolder) {
	static bool scanned = false;
	static std::vector<std::string> cached;
	if (scanned) {
		return cached;
	}
	scanned = true;

	std::vector<std::filesystem::path> roots;
	roots.emplace_back(".");
	roots.emplace_back(std::filesystem::path("CD") / "CNC95");
	if (cd_subfolder && *cd_subfolder) {
		roots.emplace_back(std::filesystem::path("CD") / cd_subfolder);
	}
	static const char* kTiberianFolders[] = {"CD1", "CD2", "CD3"};
	for (auto folder : kTiberianFolders) {
		roots.emplace_back(std::filesystem::path("CD") / "TIBERIAN_DAWN" / folder);
	}

	static const char* kAllowedMixes[] = {"GENERAL.MIX", "CONQUER.MIX", "CCLOCAL.MIX", "LOCAL.MIX",
	                                      "UPDATE.MIX",  "UPDATEC.MIX", "UPDATA.MIX",  "LANGUAGE.MIX"};
	auto is_allowed = [&](const std::string& filename) {
		for (auto allowed : kAllowedMixes) {
			if (filename == allowed) return true;
		}
		return false;
	};

	std::unordered_set<std::string> seen_names;
	auto add_if_mix = [&](const std::filesystem::path& path) {
		if (!std::filesystem::is_regular_file(path)) return;
		const std::string ext = path.extension().string();
		if (ext != ".MIX" && ext != ".mix") return;
		std::string filename = path.filename().string();
		std::transform(filename.begin(), filename.end(), filename.begin(),
		               [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
		if (!is_allowed(filename)) return;
		if (!seen_names.insert(filename).second) {
			return;
		}
		const std::string normalized = path.string();
		if (std::find(cached.begin(), cached.end(), normalized) == cached.end()) {
			cached.push_back(normalized);
		}
	};

	for (auto const& root : roots) {
		if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) continue;
		for (auto const& entry : std::filesystem::directory_iterator(root)) {
			add_if_mix(entry.path());
		}
	}

	return cached;
}

}  // namespace

void Load_Title_Screen(char* name, GraphicViewPortClass* video_page, unsigned char* palette) {
	if (!name || !video_page) return;

	DecodedPcx pcx{};
	bool loaded = false;
	const char* cd_subfolder = CDFileClass::Get_CD_Subfolder();

	static bool mixes_registered = false;
	static std::vector<std::unique_ptr<MixFileClass>> registered;
	static std::vector<std::string> registered_paths;
	if (!mixes_registered) {
		auto register_mix = [cd_subfolder](const char* filename) {
			if (!filename) return;
			std::vector<std::string> paths;
			auto add_path = [&](const std::string& path) {
				if (path.empty()) return;
				if (std::find(paths.begin(), paths.end(), path) != paths.end()) return;
				paths.emplace_back(path);
			};

			auto add_path_with_parent = [&](const std::string& path) {
				add_path(path);
				add_path(std::string("../") + path);
			};

			auto add_variants = [&](const std::string& base) {
				add_path_with_parent(base);
				std::string upper = base;
				std::transform(upper.begin(), upper.end(), upper.begin(),
				               [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
				add_path_with_parent(upper);
			};

			auto add_tiberian_paths = [&](const std::string& base) {
				static const char* kDiscs[] = {"CD1", "CD2", "CD3"};
				for (auto disc : kDiscs) {
					add_variants(std::string("CD/TIBERIAN_DAWN/") + disc + "/" + base);
				}
			};

			add_variants(filename);
			if (cd_subfolder && *cd_subfolder) {
				add_variants(std::string("CD/") + cd_subfolder + "/" + filename);
			}
			add_variants(std::string("CD/CNC95/") + filename);
			add_variants(std::string("CD/") + filename);
			add_variants(std::string("CD/GDI/") + filename);
			add_variants(std::string("CD/NOD/") + filename);
			add_tiberian_paths(filename);

			for (auto const& path : paths) {
				std::ifstream test(path, std::ios::binary);
				if (test) {
					bool already_registered = std::any_of(
					    registered.begin(), registered.end(),
					    [&](auto const& mix) { return strcasecmp(mix->Filename, path.c_str()) == 0; });
					bool path_known = std::any_of(
					    registered_paths.begin(), registered_paths.end(),
					    [&](auto const& p) { return strcasecmp(p.c_str(), path.c_str()) == 0; });
					if (!already_registered) {
						registered.emplace_back(std::make_unique<MixFileClass>(path.c_str()));
						CCDebugString("Load_Title_Screen: registered mix ");
						CCDebugString(path.c_str());
						CCDebugString("\n");
					}
					if (!path_known) {
						registered_paths.push_back(path);
					}
				}
			}
		};

		// Title art normally lives in GENERAL.MIX/CONQUER.MIX on the Win95 discs.
		register_mix("GENERAL.MIX");
		register_mix("CONQUER.MIX");
		register_mix("CCLOCAL.MIX");
		register_mix("LOCAL.MIX");
		register_mix("UPDATE.MIX");
		register_mix("UPDATEC.MIX");
		register_mix("UPDATA.MIX");
		for (auto const& path : Discover_Mix_Files(cd_subfolder)) {
			if (std::find(registered_paths.begin(), registered_paths.end(), path) == registered_paths.end()) {
				std::ifstream test(path, std::ios::binary);
				if (test) {
					registered_paths.push_back(path);
				}
			}
		}
		mixes_registered = true;
	}

	std::vector<std::string> candidates;
	auto add_candidate = [&](const std::string& value) {
		if (value.empty()) return;
		if (std::find(candidates.begin(), candidates.end(), value) == candidates.end()) {
			candidates.emplace_back(value);
		}
	};

	auto add_with_case_variants = [&](const std::string& value) {
		add_candidate(value);
		std::string upper = value;
		std::transform(upper.begin(), upper.end(), upper.begin(),
		               [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
		add_candidate(upper);
	};

	add_with_case_variants(name);
	// Also consider the alternate TITLE.* naming used by some distributions.
	add_with_case_variants("TITLE.PCX");
	add_with_case_variants("TITLE.CPS");
	const char* dot = std::strrchr(name, '.');
	if (dot) {
		std::string ext(dot);
		std::transform(ext.begin(), ext.end(), ext.begin(),
		               [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
		if (ext == ".PCX") {
			std::string cps_name(name);
			cps_name.replace(dot - name, std::strlen(dot), ".CPS");
			add_with_case_variants(cps_name);
		}
	}

	for (auto const& candidate : candidates) {
		std::vector<unsigned char> buffer;
		if (!Load_File_To_Buffer(candidate.c_str(), buffer)) {
			continue;
		}
		if (Decode_Pcx_Or_Cps(buffer, pcx)) {
			if (std::getenv("TD_DUMP_TITLE")) {
				// Dump the raw candidate file we read for later inspection.
				std::string outname = std::string("title_candidate_") + candidate;
				std::ofstream out(outname, std::ios::binary);
				if (out) {
					out.write(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
					out.close();
					CCDebugString("Wrote ");
					CCDebugString(outname.c_str());
					CCDebugString("\n");
				}
			}
			loaded = true;
			break;
		}
	}

	// Fallback: scan registered mixes for any decodable PCX/CPS payload (some distributions ship unnamed title art).
	auto try_decode_mix_blob = [&](const std::string& path) -> bool {
		std::ifstream file(path, std::ios::binary);
		if (!file) return false;

		std::uint16_t count = 0;
		std::uint32_t size = 0;
		file.read(reinterpret_cast<char*>(&count), sizeof(count));
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		if (!file) return false;

		std::vector<MixSubBlock> blocks;
		blocks.resize(count);
		for (std::uint16_t i = 0; i < count; ++i) {
			file.read(reinterpret_cast<char*>(&blocks[i]), sizeof(MixSubBlock));
		}

		const std::streamoff base = static_cast<std::streamoff>(6 + count * sizeof(MixSubBlock));
		for (auto const& block : blocks) {
			if (block.size == 0 || block.size > kMaxScanSize) continue;
			std::vector<unsigned char> blob(block.size);
			file.seekg(base + static_cast<std::streamoff>(block.offset), std::ios::beg);
			file.read(reinterpret_cast<char*>(blob.data()), static_cast<std::streamsize>(block.size));
			if (!file) break;
			if (Decode_Pcx_Or_Cps(blob, pcx) && pcx.width >= 320 && pcx.height >= 200) {
				if (std::getenv("TD_DUMP_TITLE")) {
					// Dump the raw mix block we found so it can be inspected.
					std::string outname = std::string("title_blob_") + std::to_string(block.offset) + std::string(".bin");
					std::ofstream out(outname, std::ios::binary);
					if (out) {
						out.write(reinterpret_cast<char*>(blob.data()), static_cast<std::streamsize>(blob.size()));
						out.close();
						CCDebugString("Wrote ");
						CCDebugString(outname.c_str());
						CCDebugString("\n");
					}
				}
				return true;
			}
		}
		return false;
	};

	if (!loaded) {
		for (auto const& path : registered_paths) {
			if (try_decode_mix_blob(path)) {
				loaded = true;
				break;
			}
		}
	}

	if (loaded) {
		if (pcx.has_palette) {
			Apply_Title_Palette(pcx.palette, palette ? palette : Palette);
		}

		// Optional debug dump: write the decoded image to PPM if requested.
		if (std::getenv("TD_DUMP_TITLE")) {
			std::ofstream ofs("title_dump.ppm", std::ios::binary);
			if (ofs) {
				ofs << "P6\n" << pcx.width << ' ' << pcx.height << "\n255\n";
				for (std::size_t i = 0; i < static_cast<std::size_t>(pcx.width) * pcx.height; ++i) {
					const unsigned char idx = pcx.pixels[i];
					unsigned char r = pcx.palette[idx * 3];
					unsigned char g = pcx.palette[idx * 3 + 1];
					unsigned char b = pcx.palette[idx * 3 + 2];
					// palette entries are 6-bit (0..63) — scale to 0..255 range by *4.
					ofs.put(static_cast<unsigned char>(r << 2));
					ofs.put(static_cast<unsigned char>(g << 2));
					ofs.put(static_cast<unsigned char>(b << 2));
				}
				ofs.close();
				CCDebugString("Wrote title_dump.ppm\n");
			}
		}
		Blit_With_Scale(pcx, video_page);
		Present_Title(video_page);
		return;
	}

	CCDebugString("Load_Title_Screen: failed to open title art.\n");
	GraphicBufferClass* buffer = video_page->Get_Graphic_Buffer();
	if (buffer) {
		if (!buffer->Is_Valid()) {
			buffer->Init(video_page->Get_Width(), video_page->Get_Height(), nullptr, 0, GBC_NONE);
		}
		unsigned char* dest = buffer->Get_Buffer();
		if (dest) {
			const int pitch = buffer->Get_Width();
			const int height = buffer->Get_Height();
			std::fill(dest, dest + pitch * height, 0);
		}
	}
	Present_Title(video_page);
}
