#include "legacy/wwalloc.h"
#include "legacy/error.h"

#include <cstdlib>
#include <cstring>
#include <limits>

/*
**  Modern, platform-neutral replacements for the legacy allocation helpers.
**  The original code toggled between EMS/XMS/DPMI backends; here we just wrap
**  the standard C heap and preserve the expected entry points so the rest of
**  the game can link.
*/

unsigned long MinRam = 0;
unsigned long MaxRam = 0;

extern "C" MemoryFlagType operator|(MemoryFlagType lhs, MemoryFlagType rhs) {
  return static_cast<MemoryFlagType>(
      static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

extern "C" MemoryFlagType operator&(MemoryFlagType lhs, MemoryFlagType rhs) {
  return static_cast<MemoryFlagType>(
      static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

extern "C" MemoryFlagType operator~(MemoryFlagType value) {
  return static_cast<MemoryFlagType>(~static_cast<unsigned>(value));
}

extern "C" void* Alloc(unsigned long bytes_to_alloc, MemoryFlagType flags) {
  if (bytes_to_alloc == 0) {
    bytes_to_alloc = 1;
  }

  void* ptr = std::malloc(static_cast<std::size_t>(bytes_to_alloc));
  if (!ptr) {
    if (Memory_Error) {
      Memory_Error();
    }
    return nullptr;
  }

  if ((flags & MEM_CLEAR) != 0 && ptr) {
    std::memset(ptr, 0, static_cast<std::size_t>(bytes_to_alloc));
  }

  MaxRam += bytes_to_alloc;
  if (MinRam == 0 || bytes_to_alloc < MinRam) {
    MinRam = bytes_to_alloc;
  }
  return ptr;
}

extern "C" void Free(void const* pointer) {
  if (!pointer) {
    return;
  }
  std::free(const_cast<void*>(pointer));
}

extern "C" void* Resize_Alloc(void const* original_ptr,
                             unsigned long new_size_in_bytes) {
  void* resized =
      std::realloc(const_cast<void*>(original_ptr),
                   static_cast<std::size_t>(new_size_in_bytes));
  if (!resized && new_size_in_bytes != 0 && Memory_Error) {
    Memory_Error();
  }
  return resized;
}

extern "C" long Ram_Free(MemoryFlagType /*flag*/) {
  // No direct portable way to query free system RAM; return a generous
  // placeholder so callers that gate on this value can proceed.
  return static_cast<long>(std::numeric_limits<long>::max() / 4);
}

extern "C" long Total_Ram_Free(MemoryFlagType flag) { return Ram_Free(flag); }

extern "C" long Heap_Size(MemoryFlagType /*flag*/) {
  // Track the total amount ever allocated via this wrapper. This isn't a true
  // heap size but preserves the legacy API surface.
  return static_cast<long>(MaxRam);
}
