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

#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <fstream>
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
			output.palette[index] = static_cast<unsigned char>(palette_data[index] >> 2);
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

void Blit_To_View(const DecodedPcx& pcx, GraphicViewPortClass* view) {
	if (!view) return;
	GraphicBufferClass* buffer = view->Get_Graphic_Buffer();
	if (!buffer) return;

	if (!buffer->Is_Valid() || buffer->Get_Width() < pcx.width || buffer->Get_Height() < pcx.height) {
		buffer->Init(pcx.width, pcx.height, nullptr, 0, GBC_NONE);
		view->Configure(buffer, 0, 0, pcx.width, pcx.height);
	}

	unsigned char* dest = buffer->Get_Buffer();
	if (!dest) return;

	const int copy_width = std::min(pcx.width, view->Get_Width());
	const int copy_height = std::min(pcx.height, view->Get_Height());
	const int pitch = buffer->Get_Width();
	const int origin_x = view->Get_XPos();
	const int origin_y = view->Get_YPos();

	for (int y = 0; y < copy_height; ++y) {
		const unsigned char* src_row = pcx.pixels.data() + y * pcx.width;
		unsigned char* dest_row = dest + (origin_y + y) * pitch + origin_x;
		std::memcpy(dest_row, src_row, static_cast<std::size_t>(copy_width));
	}
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
				palette[offset + 0] = static_cast<unsigned char>(value >> 2);
				palette[offset + 1] = static_cast<unsigned char>(value >> 2);
				palette[offset + 2] = static_cast<unsigned char>(value >> 2);
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
		if (palette && pcx.has_palette) {
			std::memcpy(palette, pcx.palette, kPaletteSize);
			if (GamePalette) {
				std::memcpy(GamePalette, pcx.palette, kPaletteSize);
			}
		}
		Blit_To_View(pcx, video_page);
	} else {
		CCDebugString("Load_Title_Screen: failed to open PCX.\n");
		Fill_Fallback(video_page, palette);
	}
}
