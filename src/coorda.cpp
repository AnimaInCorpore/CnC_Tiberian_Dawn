#include "legacy_compat.h"

int Cardinal_To_Fixed(int base, int cardinal) {
    if (base <= 0) return 0xFFFF;
    if (cardinal < 0) cardinal = 0;

    const uint64_t scaled = (static_cast<uint64_t>(static_cast<uint32_t>(cardinal)) << 8);
    const uint64_t result = scaled / static_cast<uint32_t>(base);
    if (result > 0xFFFFu) return 0xFFFF;
    return static_cast<int>(result);
}

int Fixed_To_Cardinal(int base, int fixed) {
    if (base < 0) base = 0;
    if (fixed < 0) fixed = 0;

    const uint64_t product = static_cast<uint64_t>(static_cast<uint32_t>(base)) * static_cast<uint32_t>(fixed);
    const uint64_t rounded = product + 0x80u;
    if (rounded > 0x00FFFFFFu) return 0xFFFF;
    return static_cast<int>(rounded >> 8);
}

