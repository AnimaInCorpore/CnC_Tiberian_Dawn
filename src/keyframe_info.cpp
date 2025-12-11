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

#include <cstring>

namespace {

struct KeyFrameHeaderType {
  unsigned short frames;
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  unsigned short largest_frame_size;
  short flags;
};

}  // namespace

unsigned short Get_Build_Frame_Count(void const* dataptr) {
  if (dataptr) {
    return static_cast<const KeyFrameHeaderType*>(dataptr)->frames;
  }
  return 0;
}

unsigned short Get_Build_Frame_X(void const* dataptr) {
  if (dataptr) {
    return static_cast<const KeyFrameHeaderType*>(dataptr)->x;
  }
  return 0;
}

unsigned short Get_Build_Frame_Y(void const* dataptr) {
  if (dataptr) {
    return static_cast<const KeyFrameHeaderType*>(dataptr)->y;
  }
  return 0;
}

unsigned short Get_Build_Frame_Width(void const* dataptr) {
  if (dataptr) {
    return static_cast<const KeyFrameHeaderType*>(dataptr)->width;
  }
  return 0;
}

unsigned short Get_Build_Frame_Height(void const* dataptr) {
  if (dataptr) {
    return static_cast<const KeyFrameHeaderType*>(dataptr)->height;
  }
  return 0;
}

bool Get_Build_Frame_Palette(void const* dataptr, void* palette) {
  if (dataptr && palette && (static_cast<const KeyFrameHeaderType*>(dataptr)->flags & 1)) {
    const char* ptr = static_cast<const char*>(
        Add_Long_To_Pointer(const_cast<void*>(dataptr),
                            (static_cast<long>(sizeof(unsigned long)) << 1) *
                                    static_cast<const KeyFrameHeaderType*>(dataptr)->frames +
                                16 + sizeof(KeyFrameHeaderType)));
    std::memcpy(palette, ptr, 768L);
    return true;
  }
  return false;
}
