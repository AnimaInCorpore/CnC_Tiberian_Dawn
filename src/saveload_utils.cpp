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

bool Read_Object(void* ptr, int base_size, int class_size, FileClass& file, void* vtable) {
	int size = 0;

	if (file.Read(&size, sizeof(size)) != static_cast<long>(sizeof(size))) {
		return false;
	}

	if (size != class_size) {
		return false;
	}

	if (file.Read(ptr, class_size) != static_cast<long>(class_size)) {
		return false;
	}

	if (vtable) {
		Set_VTable(ptr, base_size, vtable);
	}

	return true;
}

bool Write_Object(void* ptr, int class_size, FileClass& file) {
	if (file.Write(&class_size, sizeof(class_size)) != static_cast<long>(sizeof(class_size))) {
		return false;
	}

	return file.Write(ptr, class_size) == static_cast<long>(class_size);
}

void* Get_VTable(void* ptr, int base_size) {
	return reinterpret_cast<void**>(reinterpret_cast<char*>(ptr) + base_size - static_cast<int>(sizeof(void*)))[0];
}

void Set_VTable(void* ptr, int base_size, void* vtable) {
	reinterpret_cast<void**>(reinterpret_cast<char*>(ptr) + base_size - static_cast<int>(sizeof(void*)))[0] = vtable;
}
