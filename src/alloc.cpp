#include "legacy/wwalloc.h"
#include "legacy/error.h"

#include <SDL.h>

#include <algorithm>
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
static unsigned long CurrentRam = 0;

namespace {

struct AllocHeader {
  unsigned long size;
};

static_assert(sizeof(AllocHeader) <= 16, "allocation header unexpectedly large");

unsigned long Total_System_Ram_Bytes() {
  const int system_mb = SDL_GetSystemRAM();
  if (system_mb <= 0) {
    return 0;
  }
  return static_cast<unsigned long>(system_mb) * 1024ul * 1024ul;
}

AllocHeader* Header_From_User_Ptr(void const* user_ptr) {
  return reinterpret_cast<AllocHeader*>(const_cast<void*>(user_ptr)) - 1;
}

}  // namespace

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

  const std::size_t raw_size =
      static_cast<std::size_t>(bytes_to_alloc) + sizeof(AllocHeader);
  AllocHeader* header = static_cast<AllocHeader*>(std::malloc(raw_size));
  if (!header) {
    if (Memory_Error) {
      Memory_Error();
    }
    return nullptr;
  }

  header->size = bytes_to_alloc;
  void* user_ptr = header + 1;

  if ((flags & MEM_CLEAR) != 0) {
    std::memset(user_ptr, 0, static_cast<std::size_t>(bytes_to_alloc));
  }

  CurrentRam += bytes_to_alloc;
  if (CurrentRam > MaxRam) {
    MaxRam = CurrentRam;
  }
  if (MinRam == 0 || bytes_to_alloc < MinRam) {
    MinRam = bytes_to_alloc;
  }
  return user_ptr;
}

extern "C" void Free(void const* pointer) {
  if (!pointer) {
    return;
  }
  AllocHeader* header = Header_From_User_Ptr(pointer);
  if (header->size <= CurrentRam) {
    CurrentRam -= header->size;
  } else {
    CurrentRam = 0;
  }
  std::free(header);
}

extern "C" void* Resize_Alloc(void const* original_ptr,
                             unsigned long new_size_in_bytes) {
  if (!original_ptr) {
    return Alloc(new_size_in_bytes, MEM_NORMAL);
  }

  AllocHeader* old_header = Header_From_User_Ptr(original_ptr);
  const unsigned long old_size = old_header->size;
  const std::size_t raw_size =
      static_cast<std::size_t>(new_size_in_bytes) + sizeof(AllocHeader);

  AllocHeader* new_header =
      static_cast<AllocHeader*>(std::realloc(old_header, raw_size));
  if (!new_header && new_size_in_bytes != 0 && Memory_Error) {
    Memory_Error();
    return nullptr;
  }

  if (new_header) {
    new_header->size = new_size_in_bytes;
  }

  if (old_size <= CurrentRam) {
    CurrentRam -= old_size;
  } else {
    CurrentRam = 0;
  }
  CurrentRam += new_size_in_bytes;
  if (CurrentRam > MaxRam) {
    MaxRam = CurrentRam;
  }

  return new_header ? (new_header + 1) : nullptr;
}

extern "C" long Ram_Free(MemoryFlagType /*flag*/) {
  const unsigned long total = Total_System_Ram_Bytes();
  if (total == 0) {
    return static_cast<long>(std::numeric_limits<long>::max() / 4);
  }
  const unsigned long remaining = (CurrentRam < total) ? (total - CurrentRam) : 0;
  return static_cast<long>(
      std::min<unsigned long>(remaining,
                              static_cast<unsigned long>(std::numeric_limits<long>::max())));
}

extern "C" long Total_Ram_Free(MemoryFlagType flag) { return Ram_Free(flag); }

extern "C" long Heap_Size(MemoryFlagType /*flag*/) {
  return static_cast<long>(CurrentRam);
}
