#include "legacy/function.h"

#include <algorithm>

extern "C" {
extern unsigned char PaletteInterpolationTable[SIZE_OF_PALETTE][SIZE_OF_PALETTE];
extern unsigned char* InterpolationPalette;

void __cdecl Asm_Create_Palette_Interpolation_Table(void);
void __cdecl Asm_Interpolate(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                             int src_width, int dest_width);
void __cdecl Asm_Interpolate_Line_Double(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                                         int src_width, int dest_width);
void __cdecl Asm_Interpolate_Line_Interpolate(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                                              int src_width, int dest_width);
}

void __cdecl Asm_Create_Palette_Interpolation_Table(void) {
  if (!InterpolationPalette) return;

  for (int i = 0; i < SIZE_OF_PALETTE; ++i) {
    const unsigned char* first_palette_ptr = InterpolationPalette + (i * 3);
    const int first_r = first_palette_ptr[0];
    const int first_g = first_palette_ptr[1];
    const int first_b = first_palette_ptr[2];

    for (int j = 0; j < SIZE_OF_PALETTE; ++j) {
      const unsigned char* second_palette_ptr = InterpolationPalette + (j * 3);
      const int second_r = second_palette_ptr[0];
      const int second_g = second_palette_ptr[1];
      const int second_b = second_palette_ptr[2];

      const int dest_r = (first_r + second_r) >> 1;
      const int dest_g = (first_g + second_g) >> 1;
      const int dest_b = (first_b + second_b) >> 1;

      int closest_distance = 500000;
      int index_of_closest_color = 0;
      for (int p = 0; p < SIZE_OF_PALETTE; ++p) {
        const unsigned char* match_pal_ptr = InterpolationPalette + (p * 3);
        const int diff_r = match_pal_ptr[0] - dest_r;
        const int diff_g = match_pal_ptr[1] - dest_g;
        const int diff_b = match_pal_ptr[2] - dest_b;
        const int distance = diff_r * diff_r + diff_g * diff_g + diff_b * diff_b;
        if (distance < closest_distance) {
          closest_distance = distance;
          index_of_closest_color = p;
        }
      }
      PaletteInterpolationTable[i][j] = static_cast<unsigned char>(index_of_closest_color);
    }
  }
}

namespace {

void Scale_Row_Interpolated(const unsigned char* src_row, const unsigned char* next_row,
                            unsigned char* dest_row, unsigned char* dest_row2, int src_width,
                            int dest_stride, bool duplicate_lines, bool interpolate_lines) {
  for (int x = 0; x < src_width; ++x) {
    const unsigned char left = src_row[x];
    const unsigned char right = (x + 1 < src_width) ? src_row[x + 1] : left;
    dest_row[(x << 1)] = left;
    dest_row[(x << 1) + 1] = PaletteInterpolationTable[left][right];

    if (duplicate_lines) {
      dest_row2[(x << 1)] = dest_row[(x << 1)];
      dest_row2[(x << 1) + 1] = dest_row[(x << 1) + 1];
      continue;
    }

    const unsigned char down = next_row[x];
    const unsigned char down_right = (x + 1 < src_width) ? next_row[x + 1] : down;
    if (interpolate_lines) {
      dest_row2[(x << 1)] = PaletteInterpolationTable[left][down];
      dest_row2[(x << 1) + 1] =
          PaletteInterpolationTable[PaletteInterpolationTable[left][right]][PaletteInterpolationTable[down][down_right]];
    } else {
      dest_row2[(x << 1)] = down;
      dest_row2[(x << 1) + 1] = PaletteInterpolationTable[down][down_right];
    }
  }
  (void)dest_stride;
}

void Interpolate_Scale(unsigned char* src_ptr, unsigned char* dest_ptr, int lines, int src_width,
                       int dest_width, bool duplicate_lines, bool interpolate_lines) {
  if (!src_ptr || !dest_ptr || lines <= 0 || src_width <= 0 || dest_width <= 0) return;

  for (int y = 0; y < lines; ++y) {
    const unsigned char* src_row = src_ptr + (y * src_width);
    const unsigned char* next_row =
        (y + 1 < lines) ? (src_ptr + ((y + 1) * src_width)) : src_row;
    unsigned char* dest_row = dest_ptr + (y * 2 * dest_width);
    unsigned char* dest_row2 = dest_row + dest_width;
    Scale_Row_Interpolated(src_row, next_row, dest_row, dest_row2, src_width, dest_width,
                           duplicate_lines, interpolate_lines);
  }
}

}  // namespace

void __cdecl Asm_Interpolate(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                             int src_width, int dest_width) {
  Interpolate_Scale(src_ptr, dest_ptr, lines, src_width, dest_width, false, true);
}

void __cdecl Asm_Interpolate_Line_Double(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                                         int src_width, int dest_width) {
  Interpolate_Scale(src_ptr, dest_ptr, lines, src_width, dest_width, true, false);
}

void __cdecl Asm_Interpolate_Line_Interpolate(unsigned char* src_ptr, unsigned char* dest_ptr, int lines,
                                              int src_width, int dest_width) {
  Interpolate_Scale(src_ptr, dest_ptr, lines, src_width, dest_width, false, false);
}
