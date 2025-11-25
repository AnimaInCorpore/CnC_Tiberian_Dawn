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

#include "legacy/display.h"

#include "legacy/defines.h"
#include "legacy/externs.h"
#include "legacy/function.h"
#include "legacy/options.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

namespace {

constexpr int kPaletteSize = 256 * 3;
constexpr int kAllowedCount = 16;
constexpr int kAllowedStart = 256 - kAllowedCount;

std::array<unsigned char, 3> Color_From_Palette(const unsigned char* palette, int index) {
	std::array<unsigned char, 3> result{};
	if (!palette || index < 0 || index >= 256) return result;
	const int offset = index * 3;
	result[0] = palette[offset + 0];
	result[1] = palette[offset + 1];
	result[2] = palette[offset + 2];
	return result;
}

void Ensure_Palette_Buffer(unsigned char*& buffer) {
	if (!buffer) {
		buffer = new unsigned char[kPaletteSize];
		std::fill_n(buffer, kPaletteSize, 0);
	}
}

void Build_Default_Palette(unsigned char* palette) {
	if (!palette) return;
	for (int i = 0; i < 256; ++i) {
		const unsigned char value = static_cast<unsigned char>(i % 64);
		palette[i * 3 + 0] = value;
		palette[i * 3 + 1] = value;
		palette[i * 3 + 2] = value;
	}
}

void Copy_Palette(const unsigned char* source, unsigned char* dest) {
	if (!source || !dest) return;
	std::memcpy(dest, source, kPaletteSize);
}

int Closest_Color(const unsigned char* palette, int target_r, int target_g, int target_b, int start, int end) {
	int best = start;
	int best_error = std::numeric_limits<int>::max();
	for (int index = start; index < end; ++index) {
		const int offset = index * 3;
		const int dr = static_cast<int>(palette[offset + 0]) - target_r;
		const int dg = static_cast<int>(palette[offset + 1]) - target_g;
		const int db = static_cast<int>(palette[offset + 2]) - target_b;
		const int error = dr * dr + dg * dg + db * db;
		if (error < best_error) {
			best_error = error;
			best = index;
			if (error == 0) break;
		}
	}
	return best;
}

void Prepare_Global_Palettes() {
	Ensure_Palette_Buffer(GamePalette);
	Ensure_Palette_Buffer(OriginalPalette);
	Ensure_Palette_Buffer(BlackPalette);
	Ensure_Palette_Buffer(WhitePalette);
	Ensure_Palette_Buffer(Palette);

	Build_Default_Palette(GamePalette);
	Copy_Palette(GamePalette, OriginalPalette);
	std::fill_n(BlackPalette, kPaletteSize, 0);
	std::fill_n(WhitePalette, kPaletteSize, 0x3F);
	Copy_Palette(GamePalette, Palette);
}

}  // namespace

// -----------------------------------------------------------------------------.
// Palette helpers exposed through the legacy headers.
// -----------------------------------------------------------------------------
void* Build_Fading_Table(void const* palette, void* dest, int color, int frac) {
	auto* remap = static_cast<unsigned char*>(dest);
	const auto* pal = static_cast<unsigned char const*>(palette);
	if (!remap || !pal) return dest;

	frac = std::clamp(frac, 0, 0xFF);
	const auto target = Color_From_Palette(pal, color);

	remap[0] = 0;
	for (int index = 1; index < 256; ++index) {
		const auto current = Color_From_Palette(pal, index);
		const int ideal_r = current[0] - ((current[0] - target[0]) * frac) / 256;
		const int ideal_g = current[1] - ((current[1] - target[1]) * frac) / 256;
		const int ideal_b = current[2] - ((current[2] - target[2]) * frac) / 256;
		remap[index] = static_cast<unsigned char>(Closest_Color(pal, ideal_r, ideal_g, ideal_b, 0, 256));
	}
	return dest;
}

void* Conquer_Build_Fading_Table(void const* palette, void* dest, int color, int frac) {
	auto* remap = static_cast<unsigned char*>(dest);
	const auto* pal = static_cast<unsigned char const*>(palette);
	if (!remap || !pal) return dest;

	frac = std::clamp(frac, 0, 0xFF);
	const auto target = Color_From_Palette(pal, color);

	remap[0] = 0;
	for (int index = 1; index < kAllowedStart; ++index) {
		const auto current = Color_From_Palette(pal, index);
		const int ideal_r = current[0] - ((current[0] - target[0]) * frac) / 256;
		const int ideal_g = current[1] - ((current[1] - target[1]) * frac) / 256;
		const int ideal_b = current[2] - ((current[2] - target[2]) * frac) / 256;
		remap[index] = static_cast<unsigned char>(
		    Closest_Color(pal, ideal_r, ideal_g, ideal_b, kAllowedStart, 256));
	}
	for (int index = kAllowedStart; index < 256; ++index) {
		remap[index] = static_cast<unsigned char>(index);
	}
	return dest;
}

long Translucent_Table_Size(int count) { return 256L + (256L * count); }

void* Build_Translucent_Table(void const* palette, TLucentType const* control, int count, void* buffer) {
	if (!palette || !control || count <= 0) return buffer;

	if (!buffer) {
		buffer = new char[Translucent_Table_Size(count)];
	}

	if (!buffer) return buffer;

	std::memset(buffer, -1, 256);
	auto* table = static_cast<unsigned char*>(Add_Long_To_Pointer(buffer, 256));
	for (int index = 0; index < count; ++index) {
		static_cast<unsigned char*>(buffer)[control[index].SourceColor] = static_cast<unsigned char>(index);
		Build_Fading_Table(palette, table, control[index].DestColor, control[index].Fading);
		table = static_cast<unsigned char*>(Add_Long_To_Pointer(table, 256));
	}

	return buffer;
}

void* Conquer_Build_Translucent_Table(void const* palette, TLucentType const* control, int count, void* buffer) {
	if (!palette || !control || count <= 0) return buffer;

	if (!buffer) {
		buffer = new char[Translucent_Table_Size(count)];
	}

	if (!buffer) return buffer;

	std::memset(buffer, -1, 256);
	auto* table = static_cast<unsigned char*>(Add_Long_To_Pointer(buffer, 256));
	for (int index = 0; index < count; ++index) {
		static_cast<unsigned char*>(buffer)[control[index].SourceColor] = static_cast<unsigned char>(index);
		Conquer_Build_Fading_Table(palette, table, control[index].DestColor, control[index].Fading);
		table = static_cast<unsigned char*>(Add_Long_To_Pointer(table, 256));
	}

	return buffer;
}

// -----------------------------------------------------------------------------
// DisplayClass
// -----------------------------------------------------------------------------
LayerClass DisplayClass::Layer[LAYER_COUNT];

unsigned char DisplayClass::FadingBrighten[256];
unsigned char DisplayClass::FadingShade[256];
unsigned char DisplayClass::FadingLight[256];
unsigned char DisplayClass::RemapTables[HOUSE_COUNT][3][256];
unsigned char DisplayClass::FadingGreen[256];
unsigned char DisplayClass::FadingYellow[256];
unsigned char DisplayClass::FadingRed[256];
unsigned char DisplayClass::TranslucentTable[(MAGIC_COL_COUNT + 1) * 256];
unsigned char DisplayClass::WhiteTranslucentTable[(1 + 1) * 256];
unsigned char DisplayClass::MouseTranslucentTable[(4 + 1) * 256];
void const* DisplayClass::TransIconset = nullptr;
void const* DisplayClass::ShadowShapes = nullptr;
unsigned char DisplayClass::ShadowTrans[(SHADOW_COL_COUNT + 1) * 256];
unsigned char DisplayClass::UnitShadow[(USHADOW_COL_COUNT + 1) * 256];
unsigned char DisplayClass::SpecialGhost[2 * 256];

BooleanVectorClass DisplayClass::CellRedraw;
DisplayClass::TacticalClass DisplayClass::TacButton;

DisplayClass::DisplayClass(void)
    : Theater(THEATER_NONE),
      TacticalCoord(0),
      TacLeptonWidth(0),
      TacLeptonHeight(0),
      ZoneCell(0),
      ZoneOffset(0),
      CursorSize(nullptr),
      ProximityCheck(false),
      PendingObjectPtr(nullptr),
      PendingObject(nullptr),
      PendingHouse(HOUSE_NONE) {}

void DisplayClass::One_Time(void) {
	MapClass::One_Time();
	CellRedraw.Resize(MAP_CELL_TOTAL);
	for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; ++layer) {
		Layer[layer].Init();
	}
}

void DisplayClass::Init_Clear(void) {
	CellRedraw.Clear();
	MapClass::Init_Clear();
}

void DisplayClass::Init_IO(void) { MapClass::Init_IO(); }

void DisplayClass::Init_Theater(TheaterType theater) {
	static TLucentType const MouseCols[4] = {
	    {BLACK, BLACK, 110, 0},
	    {WHITE, WHITE, 110, 0},
	    {LTGREY, LTGREY, 110, 0},
	    {DKGREY, DKGREY, 110, 0},
	};
	static TLucentType const MagicCols[MAGIC_COL_COUNT] = {
	    {32, 32, 110, 0},   {33, 33, 110, 0},   {34, 34, 110, 0},   {35, 35, 110, 0},
	    {36, 36, 110, 0},   {37, 37, 110, 0},   {38, 38, 110, 0},   {39, 39, 110, 0},
	    {BLACK, BLACK, 200, 0}, {WHITE, BLACK, 40, 0}, {LTGREY, BLACK, 80, 0}, {DKGREY, BLACK, 140, 0},
	};
	static TLucentType const WhiteCols[1] = {
	    {1, WHITE, 80, 0},
	};
	static TLucentType const ShadowCols[SHADOW_COL_COUNT] = {
	    {WHITE + 1, BLACK, 130, 0},
	    {WHITE, BLACK, 170, 0},
	    {LTGRAY, BLACK, 250, 0},
	    {DKGRAY, BLACK, 250, 0},
	};
	static TLucentType const UShadowCols[USHADOW_COL_COUNT] = {
	    {LTGREEN, BLACK, 130, 0},
	};

	MapClass::Init_Theater(theater);
	Theater = theater;

	Prepare_Global_Palettes();

	Build_Fading_Table(GamePalette, FadingGreen, GREEN, 110);
	if (theater == THEATER_DESERT) {
		FadingGreen[196] = 160;
	}
	Build_Fading_Table(GamePalette, FadingYellow, YELLOW, 140);
	Build_Fading_Table(GamePalette, FadingRed, RED, 140);
	Build_Translucent_Table(GamePalette, &MouseCols[0], 4, MouseTranslucentTable);
	Build_Translucent_Table(GamePalette, &MagicCols[0], MAGIC_COL_COUNT, TranslucentTable);
	Build_Translucent_Table(GamePalette, &WhiteCols[0], 1, WhiteTranslucentTable);
	Build_Translucent_Table(GamePalette, &ShadowCols[0], SHADOW_COL_COUNT, ShadowTrans);
	Conquer_Build_Translucent_Table(GamePalette, &UShadowCols[0], USHADOW_COL_COUNT, UnitShadow);
	Conquer_Build_Fading_Table(GamePalette, FadingShade, BLACK, 150);
	Conquer_Build_Fading_Table(GamePalette, FadingLight, WHITE, 85);
	Conquer_Build_Fading_Table(GamePalette, &SpecialGhost[256], BLACK, 100);
	for (int index = 0; index < 256; ++index) {
		SpecialGhost[index] = 0;
	}
	Build_Fading_Table(GamePalette, FadingBrighten, WHITE, 25);

	Copy_Palette(GamePalette, OriginalPalette);
	Copy_Palette(GamePalette, Palette);

	Options.Fixup_Palette();
	LastTheater = Theater;
}

void DisplayClass::AI(KeyNumType&, int, int) {}
void DisplayClass::Draw_It(bool) {}
bool DisplayClass::Map_Cell(CELL, HouseClass*) { return false; }
CELL DisplayClass::Click_Cell_Calc(int, int) { return 0; }
bool DisplayClass::Scroll_Map(DirType, int& distance, bool) {
	distance = 0;
	return false;
}
void DisplayClass::Refresh_Cells(CELL, short const*) {}
void DisplayClass::Set_View_Dimensions(int, int, int, int) {}
void DisplayClass::Set_Tactical_Position(COORDINATE coord) { TacticalCoord = coord; }
void DisplayClass::Cursor_Mark(CELL, bool) {}
void DisplayClass::Set_Cursor_Shape(short const*) {}
CELL DisplayClass::Set_Cursor_Pos(CELL pos) { return pos; }
void DisplayClass::Get_Occupy_Dimensions(int& w, int& h, short const*) {
	w = 0;
	h = 0;
}
void DisplayClass::Refresh_Band() {}
void DisplayClass::Select_These(COORDINATE, COORDINATE) {}
COORDINATE DisplayClass::Pixel_To_Coord(int, int) { return 0; }
bool DisplayClass::Coord_To_Pixel(COORDINATE coord, int& x, int& y) {
	x = 0;
	y = 0;
	return coord != 0;
}
bool DisplayClass::Push_Onto_TacMap(COORDINATE& source, COORDINATE& dest) {
	dest = source;
	return true;
}
void DisplayClass::Remove(ObjectClass const*, LayerType) {}
void DisplayClass::Submit(ObjectClass const*, LayerType) {}
CELL DisplayClass::Calculated_Cell(SourceType, HousesType) { return 0; }
bool DisplayClass::In_View(CELL) { return false; }
bool DisplayClass::Passes_Proximity_Check(ObjectTypeClass const*) { return true; }
ObjectClass* DisplayClass::Cell_Object(CELL, int, int) { return nullptr; }
ObjectClass* DisplayClass::Next_Object(ObjectClass*) { return nullptr; }
ObjectClass* DisplayClass::Prev_Object(ObjectClass*) { return nullptr; }
int DisplayClass::Cell_Shadow(CELL) { return 0; }
short const* DisplayClass::Text_Overlap_List(char const*, int, int, int) { return nullptr; }
bool DisplayClass::Is_Spot_Free(COORDINATE) const { return true; }
COORDINATE DisplayClass::Closest_Free_Spot(COORDINATE coord, bool) const { return coord; }
void DisplayClass::Sell_Mode_Control(int) {}
void DisplayClass::Repair_Mode_Control(int) {}
MouseType DisplayClass::Get_Mouse_Shape() const { return MOUSE_NORMAL; }
void DisplayClass::Mouse_Left_Release(CELL, int, int, ObjectClass*, ActionType, bool) {}
void DisplayClass::Mouse_Left_Up(bool, ObjectClass*, ActionType, bool) {}
void DisplayClass::Mouse_Left_Press(int, int) {}
void DisplayClass::Mouse_Left_Held(int, int) {}
void DisplayClass::Mouse_Right_Press() {}
void DisplayClass::Code_Pointers() {}
void DisplayClass::Decode_Pointers() {}

void Fade_Palette_To(unsigned char* target_palette, int speed, void (*callback)(void)) {
	if (!target_palette) return;

	Prepare_Global_Palettes();

	const int steps = std::clamp(SlowPalette ? std::max(1, speed / std::max(1, TIMER_SECOND / 32)) : 1, 1, 64);
	std::array<unsigned char, kPaletteSize> start{};
	Copy_Palette(Palette, start.data());

	for (int step = 1; step <= steps; ++step) {
		for (int index = 0; index < kPaletteSize; ++index) {
			const int delta = static_cast<int>(target_palette[index]) - static_cast<int>(start[index]);
			Palette[index] =
			    static_cast<unsigned char>(start[index] + (delta * step) / steps);
		}
		if (callback) {
			callback();
		}
	}

	Copy_Palette(target_palette, Palette);
	Copy_Palette(Palette, GamePalette);
}
