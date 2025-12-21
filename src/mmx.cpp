/* Ported from MMX.ASM -- CPUID-based MMX feature detection. */

#include "legacy/mmx.h"

#include <cstdint>

namespace {
struct CpuIdResult {
  std::uint32_t eax;
  std::uint32_t ebx;
  std::uint32_t ecx;
  std::uint32_t edx;
};

static inline CpuIdResult CpuId(std::uint32_t leaf) {
  CpuIdResult out{0, 0, 0, 0};
#if defined(__i386__) || defined(__x86_64__)
  #if defined(_MSC_VER)
    int regs[4]{0, 0, 0, 0};
    __cpuid(regs, static_cast<int>(leaf));
    out.eax = static_cast<std::uint32_t>(regs[0]);
    out.ebx = static_cast<std::uint32_t>(regs[1]);
    out.ecx = static_cast<std::uint32_t>(regs[2]);
    out.edx = static_cast<std::uint32_t>(regs[3]);
  #elif defined(__GNUC__) || defined(__clang__)
    std::uint32_t a = 0, b = 0, c = 0, d = 0;
    __asm__ volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf));
    out.eax = a;
    out.ebx = b;
    out.ecx = c;
    out.edx = d;
  #endif
#endif
  return out;
}
} // namespace

bool __cdecl Detect_MMX_Availability(void) {
#if defined(__i386__) || defined(__x86_64__)
  auto max_leaf = CpuId(0).eax;
  if (max_leaf < 1) {
    return false;
  }
  auto features = CpuId(1);
  return (features.edx & (1u << 23)) != 0;
#else
  return false;
#endif
}

void __cdecl Init_MMX(void) {
  // The original Win95 build patched shape blitters to MMX-optimized versions.
  // The SDL2 port does not currently ship MMX-specific blitters, so there is
  // nothing to patch here.
}

