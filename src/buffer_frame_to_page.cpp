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

#include "legacy/function.h"

#include <algorithm>
#include <cstdarg>
#include <cstddef>

namespace {

unsigned char Apply_Translucent_Table(unsigned char src_color, unsigned char dst_color,
                                      void const* table) {
  if (!table) return src_color;
  const auto* bytes = static_cast<const unsigned char*>(table);
  const unsigned char entry = bytes[src_color];
  if (entry == 0xFF) return src_color;
  return bytes[256 + static_cast<int>(entry) * 256 + dst_color];
}

}  // namespace

extern "C" long __cdecl Buffer_Frame_To_Page(int x, int y, int w, int h, void* buffer,
                                             GraphicViewPortClass& view, int flags, ...) {
  if (!buffer || w <= 0 || h <= 0) return 0;

  unsigned char const* ghostdata = nullptr;
  unsigned char const* fadingdata = nullptr;
  int fading_num = 0;
  int predoffset = 0;

  va_list ap;
  va_start(ap, flags);
  if ((flags & (SHAPE_GHOST | SHAPE_FADING)) == (SHAPE_GHOST | SHAPE_FADING)) {
    ghostdata = static_cast<unsigned char const*>(va_arg(ap, void*));
    fadingdata = static_cast<unsigned char const*>(va_arg(ap, void*));
    fading_num = va_arg(ap, int);
    predoffset = va_arg(ap, int);
  } else if ((flags & SHAPE_FADING) != 0) {
    fadingdata = static_cast<unsigned char const*>(va_arg(ap, void*));
    fading_num = va_arg(ap, int);
    predoffset = va_arg(ap, int);
  } else if ((flags & SHAPE_PREDATOR) != 0) {
    predoffset = va_arg(ap, int);
  } else {
    ghostdata = static_cast<unsigned char const*>(va_arg(ap, void*));
    predoffset = va_arg(ap, int);
  }
  va_end(ap);

  const int clip_x0 = 0;
  const int clip_y0 = 0;
  const int clip_x1 = view.Get_Width();
  const int clip_y1 = view.Get_Height();
  if (clip_x1 <= clip_x0 || clip_y1 <= clip_y0) return 0;

  unsigned char* dst = static_cast<unsigned char*>(view.Get_Offset());
  if (!dst) return 0;

  const int dst_pitch = view.Get_Pitch();
  const auto* src = static_cast<unsigned char const*>(buffer);

  const int start_x = std::max(clip_x0, x);
  const int start_y = std::max(clip_y0, y);
  const int end_x = std::min(clip_x1, x + w);
  const int end_y = std::min(clip_y1, y + h);
  if (start_x >= end_x || start_y >= end_y) return 0;

  const bool transparent = (flags & SHAPE_TRANS) != 0;
  const bool ghost = (flags & SHAPE_GHOST) != 0;
  const bool fading = (flags & SHAPE_FADING) != 0;
  const bool predator = (flags & SHAPE_PREDATOR) != 0;

  int const* predator_offsets = nullptr;
  unsigned predator_cycle = 0;
  if (predator) {
    static constexpr int kPredatorOffsetsPos[8] = {1, 3, 2, 5, 2, 3, 4, 1};
    static constexpr int kPredatorOffsetsNeg[8] = {-1, -3, -2, -5, -2, -4, -3, -1};
    if (predoffset < 0) {
      predator_offsets = kPredatorOffsetsNeg;
      predoffset = -predoffset;
    } else {
      predator_offsets = kPredatorOffsetsPos;
    }
    predator_cycle = static_cast<unsigned>(predoffset) & 7u;
  }

  for (int dy = start_y; dy < end_y; ++dy) {
    const int sy = dy - y;
    const std::size_t src_row_base = static_cast<std::size_t>(sy) * static_cast<std::size_t>(w);
    unsigned char* dst_row = dst + dy * dst_pitch;

    for (int dx = start_x; dx < end_x; ++dx) {
      const int sx = dx - x;
      unsigned char src_color = src[src_row_base + static_cast<std::size_t>(sx)];
      if (transparent && src_color == 0) continue;

      if (predator && predator_offsets) {
        int sample_x = dx + predator_offsets[predator_cycle];
        if (sample_x < clip_x0 || sample_x >= clip_x1) sample_x = dx;
        src_color = dst_row[sample_x];
        predator_cycle = (predator_cycle + 1u) & 7u;
      }

      if (fading && fadingdata) {
        int iterations = std::max(1, fading_num);
        for (int i = 0; i < iterations; ++i) {
          src_color = fadingdata[src_color];
        }
      }

      unsigned char* dst_pixel = dst_row + dx;
      const unsigned char dst_color = *dst_pixel;
      if ((ghost || transparent) && ghostdata) {
        src_color = Apply_Translucent_Table(src_color, dst_color, ghostdata);
      }
      *dst_pixel = src_color;
    }
  }

  return 0;
}

