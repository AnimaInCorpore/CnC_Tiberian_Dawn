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

/* $Header:   F:\projects\c&c\vcs\code\heap.cpv   2.18   16 Oct 1995 16:49:56   JOE_BOSTIC  $ */

#include "function.h"
#include "heap.h"

#include <cstring>
#include <new>

FixedHeapClass::FixedHeapClass(int size)
{
    Size = size;
    Buffer = 0;
    IsAllocated = false;
    TotalCount = 0;
    ActiveCount = 0;
}

FixedHeapClass::~FixedHeapClass(void) { FixedHeapClass::Clear(); }

int FixedHeapClass::Set_Heap(int count, void* buffer)
{
    Clear();

    if (!Size) return false;
    if (!count) return true;

    if (FreeFlag.Resize(count)) {
        if (!buffer) {
            buffer = new (std::nothrow) char[count * Size];
            if (!buffer) {
                FreeFlag.Clear();
                return false;
            }
            IsAllocated = true;
        }
        Buffer = buffer;
        TotalCount = count;
        return true;
    }
    return false;
}

void* FixedHeapClass::Allocate(void)
{
    if (ActiveCount < TotalCount) {
        int index = FreeFlag.First_False();

        if (index != -1) {
            ActiveCount++;
            FreeFlag[index] = true;
            return (*this)[index];
        }
    }
    return 0;
}

int FixedHeapClass::Free(void* pointer)
{
    if (pointer && ActiveCount) {
        int index = ID(pointer);

        if ((unsigned)index < (unsigned)TotalCount) {
            if (FreeFlag[index]) {
                ActiveCount--;
                FreeFlag[index] = false;
                return true;
            }
        }
    }
    return false;
}

int FixedHeapClass::ID(void const* pointer)
{
    if (pointer && Size) {
        return (int)(((char*)pointer - (char*)Buffer) / Size);
    }
    return -1;
}

void FixedHeapClass::Clear(void)
{
    if (Buffer && IsAllocated) {
        delete[] static_cast<char*>(Buffer);
    }
    Buffer = 0;
    IsAllocated = false;
    ActiveCount = 0;
    TotalCount = 0;
    FreeFlag.Clear();
}

int FixedHeapClass::Free_All(void)
{
    ActiveCount = 0;
    FreeFlag.Reset();
    return true;
}

int FixedIHeapClass::Free_All(void)
{
    ActivePointers.Delete_All();
    return FixedHeapClass::Free_All();
}

void FixedIHeapClass::Clear(void)
{
    FixedHeapClass::Clear();
    ActivePointers.Clear();
}

int FixedIHeapClass::Set_Heap(int count, void* buffer)
{
    Clear();
    if (FixedHeapClass::Set_Heap(count, buffer)) {
        ActivePointers.Resize(count);
        return true;
    }
    return false;
}

void* FixedIHeapClass::Allocate(void)
{
    void* ptr = FixedHeapClass::Allocate();
    if (ptr) {
        ActivePointers.Add(ptr);
        std::memset(ptr, 0, static_cast<size_t>(Size));
    }
    return ptr;
}

int FixedIHeapClass::Free(void* pointer)
{
    if (FixedHeapClass::Free(pointer)) {
        ActivePointers.Delete(pointer);
    }
    return false;
}

