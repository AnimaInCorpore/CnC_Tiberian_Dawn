/*
**	Command & Conquer(tm) — DPMI portability layer
*/

#include "function.h"
#include "dpmi.h"
#include <cstring>
#include <vector>

void DOSSegmentClass::Swap(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size)
{
    if (!size) return;
    std::vector<char> tmp(size);
    void *s_ptr = reinterpret_cast<void*>((uintptr_t)(src.Selector + soffset));
    void *d_ptr = reinterpret_cast<void*>((uintptr_t)(dest.Selector + doffset));

    std::memcpy(tmp.data(), s_ptr, size);
    std::memcpy(s_ptr, d_ptr, size);
    std::memcpy(d_ptr, tmp.data(), size);
}
