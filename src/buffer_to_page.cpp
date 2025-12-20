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
#include <cstring>

// Copy a raw 8-bit buffer into the destination viewport, honoring simple bounds
// clipping so callers can pass oversized rectangles without corrupting memory.
void Buffer_To_Page(int x, int y, int width, int height, void const* src, GraphicBufferClass& dest) {
  if (!src || !dest.Is_Valid()) return;

  const int dest_width = dest.Get_Width();
  const int dest_height = dest.Get_Height();
  if (width <= 0 || height <= 0 || dest_width <= 0 || dest_height <= 0) return;

  const int start_x = std::max(0, x);
  const int start_y = std::max(0, y);
  const int end_x = std::min(dest_width, x + width);
  const int end_y = std::min(dest_height, y + height);
  if (start_x >= end_x || start_y >= end_y) return;

  const auto* src_bytes = static_cast<unsigned char const*>(src);
  auto* dst_bytes = dest.Get_Buffer();

  const int copy_width = end_x - start_x;
  const int src_row_stride = width;
  const int dest_row_stride = dest_width;

  const int src_x_offset = start_x - x;
  const int src_y_offset = start_y - y;

  for (int row = start_y; row < end_y; ++row) {
    const int src_row = src_y_offset + (row - start_y);
    const unsigned char* src_row_ptr = src_bytes + (src_row * src_row_stride) + src_x_offset;
    unsigned char* dst_row_ptr = dst_bytes + (row * dest_row_stride) + start_x;
    std::memcpy(dst_row_ptr, src_row_ptr, static_cast<std::size_t>(copy_width));
  }
}

void Buffer_To_Page(int x, int y, int width, int height, void const* src, GraphicViewPortClass& dest) {
  auto* buffer = dest.Get_Graphic_Buffer();
  if (!buffer) return;
  const int offset_x = dest.Get_XPos();
  const int offset_y = dest.Get_YPos();
  Buffer_To_Page(x + offset_x, y + offset_y, width, height, src, *buffer);
}

void Buffer_To_Page(int x, int y, int width, int height, void const* src, GraphicViewPortClass* dest) {
  if (!dest) return;
  Buffer_To_Page(x, y, width, height, src, *dest);
}
