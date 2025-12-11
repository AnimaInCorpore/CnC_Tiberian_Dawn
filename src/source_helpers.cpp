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

SourceType Source_From_Name(char const* name) {
  if (name) {
    for (SourceType source = SOURCE_FIRST; source < SOURCE_COUNT; source++) {
      if (stricmp(SourceName[source], name) == 0) {
        return source;
      }
    }
  }
  return SOURCE_NONE;
}

char const* Name_From_Source(SourceType source) {
  if (static_cast<unsigned>(source) < SOURCE_COUNT) {
    return SourceName[source];
  }
  return "None";
}
