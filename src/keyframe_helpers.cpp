#include "legacy/function.h"

#include <SDL.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <vector>

extern "C" bool IsTheaterShape;

namespace {

static bool UseBigShapeBuffer = false;
static bool OriginalUseBigShapeBuffer = false;
static int LastFrameLength = 0;

constexpr unsigned short kUncompressMagicNumber = 56789;
constexpr std::size_t kInitialBigShapeBufferSize = 12000u * 1024u;
constexpr std::size_t kInitialTheaterShapeBufferSize = 1000u * 1024u;
constexpr std::size_t kBigShapeBufferGrowSize = 200u * 1024u;
constexpr std::size_t kMinFreeSpaceBeforeGrow = 128u * 1024u;

constexpr int kMaxSlots = 1500;
constexpr int kTheaterSlotStart = 1000;

struct CachedShapeHeader {
  std::uint32_t tag;
  std::int32_t draw_flags;  // kept for parity with the original layout (-1 => headers pending)
  std::uint32_t buffer_kind;  // 0 = big, 1 = theater
  std::uint32_t data_offset;  // relative to the chosen buffer start
  std::uint32_t data_length;
};

constexpr std::uint32_t kCachedShapeTag = 0x54444353u;  // 'SCDT' (arbitrary, internal-only)

static std::unique_ptr<unsigned char[]> BigShapeBuffer;
static std::unique_ptr<unsigned char[]> TheaterShapeBuffer;
static std::size_t BigShapeBufferLength = 0;
static std::size_t TheaterShapeBufferLength = 0;
static std::size_t BigShapeBufferUsed = 0;
static std::size_t TheaterShapeBufferUsed = 0;

static std::vector<std::vector<std::uint32_t>> KeyFrameSlots(kMaxSlots);
static int TotalSlotsUsed = 0;
static int TheaterSlotsUsed = kTheaterSlotStart;

struct KeyFrameHeaderType {
  unsigned short frames;
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  unsigned short largest_frame_size;
  short flags;
};

inline std::size_t Align4(std::size_t value) {
  return (value + 3u) & ~std::size_t{3u};
}

inline const unsigned char* BytePtr(const void* p) {
  return static_cast<const unsigned char*>(p);
}

inline unsigned char* BytePtr(void* p) { return static_cast<unsigned char*>(p); }

// Westwood "Format80" (LCW) decompression.
// Ported from the behavior used by C&C95 and mirrored by OpenRA's LCW decoder.
static unsigned long LCW_Uncompress(const void* source, void* dest, unsigned long dest_size) {
  if (!source || !dest || dest_size == 0) return 0;

  const unsigned char* src = BytePtr(source);
  unsigned char* out = BytePtr(dest);
  unsigned long out_index = 0;

  auto replicate_previous = [&](unsigned long src_index, unsigned long count) {
    for (unsigned long i = 0; i < count; ++i) {
      if (out_index - src_index == 1) {
        out[out_index + i] = out[out_index - 1];
      } else {
        out[out_index + i] = out[src_index + i];
      }
    }
  };

  while (true) {
    const unsigned char i = *src++;
    if ((i & 0x80u) == 0) {
      // Case 2: short back-reference.
      const unsigned char second = *src++;
      const unsigned long count = ((i & 0x70u) >> 4) + 3;
      const unsigned long rpos = ((i & 0x0Fu) << 8) + second;
      if (out_index + count > dest_size) return out_index;
      if (rpos > out_index) return out_index;
      const unsigned long src_index = out_index - rpos;
      replicate_previous(src_index, count);
      out_index += count;
    } else if ((i & 0x40u) == 0) {
      // Case 1: literal bytes.
      const unsigned long count = (i & 0x3Fu);
      if (count == 0) return out_index;  // terminator
      if (out_index + count > dest_size) return out_index;
      std::memcpy(out + out_index, src, count);
      src += count;
      out_index += count;
    } else {
      const unsigned long count3 = (i & 0x3Fu);
      if (count3 == 0x3Eu) {
        // Case 4: RLE fill.
        const unsigned long count = static_cast<unsigned long>(src[0]) |
                                    (static_cast<unsigned long>(src[1]) << 8);
        src += 2;
        const unsigned char color = *src++;
        if (out_index + count > dest_size) return out_index;
        std::memset(out + out_index, color, count);
        out_index += count;
      } else {
        // Case 3/5: long back-reference.
        const unsigned long count = (count3 == 0x3Fu)
                                        ? (static_cast<unsigned long>(src[0]) |
                                           (static_cast<unsigned long>(src[1]) << 8))
                                        : (count3 + 3);
        if (count3 == 0x3Fu) src += 2;
        const unsigned long src_index = static_cast<unsigned long>(src[0]) |
                                        (static_cast<unsigned long>(src[1]) << 8);
        src += 2;
        if (src_index >= out_index) return out_index;
        if (out_index + count > dest_size) return out_index;
        for (unsigned long end = out_index + count; out_index < end; ++out_index) {
          out[out_index] = out[src_index + (out_index - (end - count))];
        }
      }
    }
  }
}

// Westwood "Format40" XOR delta stream application.
// Mutates dest in-place by XOR'ing streamed bytes/runs into it.
static void Apply_XOR_Delta(void* dest, const void* delta) {
  if (!dest || !delta) return;
  unsigned char* out = BytePtr(dest);
  const unsigned char* src = BytePtr(delta);
  unsigned long out_index = 0;

  while (true) {
    const unsigned char i = *src++;
    if ((i & 0x80u) == 0) {
      unsigned long count = (i & 0x7Fu);
      if (count == 0) {
        // Case 6: XOR-run of single value.
        count = *src++;
        const unsigned char value = *src++;
        for (unsigned long end = out_index + count; out_index < end; ++out_index) {
          out[out_index] ^= value;
        }
      } else {
        // Case 5: XOR explicit bytes.
        for (unsigned long end = out_index + count; out_index < end; ++out_index) {
          out[out_index] ^= *src++;
        }
      }
    } else {
      unsigned long count = (i & 0x7Fu);
      if (count == 0) {
        // Extended command.
        count = static_cast<unsigned long>(src[0]) | (static_cast<unsigned long>(src[1]) << 8);
        src += 2;
        if (count == 0) return;

        if ((count & 0x8000u) == 0) {
          // Case 2: skip.
          out_index += (count & 0x7FFFu);
        } else if ((count & 0x4000u) == 0) {
          // Case 3: XOR explicit bytes.
          for (unsigned long end = out_index + (count & 0x3FFFu); out_index < end; ++out_index) {
            out[out_index] ^= *src++;
          }
        } else {
          // Case 4: XOR-run of single value.
          const unsigned char value = *src++;
          for (unsigned long end = out_index + (count & 0x3FFFu); out_index < end; ++out_index) {
            out[out_index] ^= value;
          }
        }
      } else {
        // Case 1: short skip.
        out_index += count;
      }
    }
  }
}

}  // namespace

void Check_Use_Compressed_Shapes(void) {
  const int system_mb = SDL_GetSystemRAM();
  const bool allow_uncompressed = (system_mb > 16);
  UseBigShapeBuffer = allow_uncompressed;
  OriginalUseBigShapeBuffer = UseBigShapeBuffer;
}

void Reset_Theater_Shapes(void) {
  for (int i = kTheaterSlotStart; i < TheaterSlotsUsed && i < static_cast<int>(KeyFrameSlots.size()); ++i) {
    KeyFrameSlots[static_cast<std::size_t>(i)].clear();
  }
  TheaterShapeBufferUsed = 0;
  TheaterSlotsUsed = kTheaterSlotStart;
}

void Disable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = false;
}

void Enable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = OriginalUseBigShapeBuffer;
}

void* Get_Shape_Header_Data(void* ptr) {
  if (!ptr) return nullptr;
  const auto* header = static_cast<const CachedShapeHeader*>(ptr);
  if (header->tag != kCachedShapeTag) return ptr;

  const unsigned char* base = nullptr;
  if (header->buffer_kind == 1u) {
    base = TheaterShapeBuffer.get();
  } else {
    base = BigShapeBuffer.get();
  }
  if (!base) return ptr;
  return const_cast<unsigned char*>(base + header->data_offset);
}

int Get_Last_Frame_Length(void) {
  return LastFrameLength;
}

void* Build_Frame(void const* dataptr, unsigned short framenumber, void* buffptr) {
  LastFrameLength = 0;
  if (!dataptr || !buffptr) return nullptr;

  auto* keyfr = const_cast<KeyFrameHeaderType*>(static_cast<const KeyFrameHeaderType*>(dataptr));
  if (framenumber >= keyfr->frames) return nullptr;

  const unsigned long buffsize =
      static_cast<unsigned long>(keyfr->width) * static_cast<unsigned long>(keyfr->height);
  if (buffsize == 0) return nullptr;

  auto ensure_buffers = [&]() -> bool {
    if (BigShapeBuffer && TheaterShapeBuffer) return true;

    if (!BigShapeBuffer) {
      BigShapeBufferLength = kInitialBigShapeBufferSize;
      BigShapeBufferUsed = 0;
      BigShapeBuffer.reset(new (std::nothrow) unsigned char[BigShapeBufferLength]);
      if (!BigShapeBuffer) {
        BigShapeBufferLength = 0;
        return false;
      }
    }

    if (!TheaterShapeBuffer) {
      TheaterShapeBufferLength = kInitialTheaterShapeBufferSize;
      TheaterShapeBufferUsed = 0;
      TheaterShapeBuffer.reset(new (std::nothrow) unsigned char[TheaterShapeBufferLength]);
      if (!TheaterShapeBuffer) {
        TheaterShapeBufferLength = 0;
        return false;
      }
    }

    return true;
  };

  auto grow_big_buffer = [&](std::size_t needed) -> bool {
    if (needed <= BigShapeBufferLength) return true;
    const std::size_t grow_by = std::max(kBigShapeBufferGrowSize, needed - BigShapeBufferLength);
    const std::size_t new_len = BigShapeBufferLength + grow_by;
    std::unique_ptr<unsigned char[]> replacement(new (std::nothrow) unsigned char[new_len]);
    if (!replacement) return false;
    if (BigShapeBufferUsed) {
      std::memcpy(replacement.get(), BigShapeBuffer.get(), BigShapeBufferUsed);
    }
    BigShapeBuffer = std::move(replacement);
    BigShapeBufferLength = new_len;
    return true;
  };

  auto slots_ready = [&]() -> bool {
    if (!UseBigShapeBuffer) return false;
    if (!ensure_buffers()) return false;

    if (keyfr->x != kUncompressMagicNumber) {
      keyfr->x = kUncompressMagicNumber;
      const int slot = IsTheaterShape ? TheaterSlotsUsed++ : TotalSlotsUsed++;
      if (slot < 0 || slot >= static_cast<int>(KeyFrameSlots.size())) {
        UseBigShapeBuffer = false;
        return false;
      }
      keyfr->y = static_cast<unsigned short>(slot);
      KeyFrameSlots[static_cast<std::size_t>(slot)].assign(keyfr->frames, 0u);
    }
    return true;
  };

  auto maybe_cache_hit = [&]() -> void* {
    if (!slots_ready()) return nullptr;

    const std::size_t slot = keyfr->y;
    if (slot >= KeyFrameSlots.size()) return nullptr;
    auto& frames = KeyFrameSlots[slot];
    if (framenumber >= frames.size()) return nullptr;

    const std::uint32_t stored = frames[framenumber];
    if (stored == 0u) return nullptr;
    const std::size_t header_offset = static_cast<std::size_t>(stored - 1u);
    unsigned char* base = IsTheaterShape ? TheaterShapeBuffer.get() : BigShapeBuffer.get();
    std::size_t base_len = IsTheaterShape ? TheaterShapeBufferLength : BigShapeBufferLength;
    if (!base) return nullptr;
    if (header_offset + sizeof(CachedShapeHeader) > base_len) return nullptr;

    auto* header = reinterpret_cast<CachedShapeHeader*>(base + header_offset);
    if (header->tag != kCachedShapeTag) return nullptr;
    if (header->data_offset + header->data_length > base_len) return nullptr;

    std::memcpy(buffptr, base + header->data_offset,
                std::min<std::size_t>(header->data_length, static_cast<std::size_t>(buffsize)));
    if (header->data_length < buffsize) {
      std::memset(static_cast<unsigned char*>(buffptr) + header->data_length, 0,
                  static_cast<std::size_t>(buffsize) - header->data_length);
    }
    LastFrameLength = static_cast<int>(header->data_length);
    return header;
  };

  if (void* cached = maybe_cache_hit()) {
    return cached;
  }

  constexpr int kSubframeOffsets = 7;  // 3.5 frames worth (2 offsets/frame).
  unsigned long offset[kSubframeOffsets] = {};

  const char* ptr =
      static_cast<const char*>(Add_Long_To_Pointer(const_cast<void*>(dataptr),
                                                   (static_cast<unsigned long>(framenumber) << 3) +
                                                       sizeof(KeyFrameHeaderType)));
  Mem_Copy(&offset[0], ptr, 12u);
  const unsigned char frameflags = static_cast<unsigned char>(offset[0] >> 24);

  const bool has_palette = (keyfr->flags & 1) != 0;

  if ((frameflags & KF_KEYFRAME) != 0) {
    const char* frame_ptr = static_cast<const char*>(
        Add_Long_To_Pointer(const_cast<void*>(dataptr), (offset[0] & 0x00FFFFFFUL)));
    if (has_palette) frame_ptr = static_cast<const char*>(Add_Long_To_Pointer(const_cast<char*>(frame_ptr), 768L));
    const unsigned long written = LCW_Uncompress(frame_ptr, buffptr, buffsize);
    LastFrameLength = static_cast<int>(written);

    if (slots_ready()) {
      const std::size_t length = static_cast<std::size_t>(std::min<unsigned long>(written, buffsize));
      const std::size_t header_offset =
          Align4(IsTheaterShape ? TheaterShapeBufferUsed : BigShapeBufferUsed);
      const std::size_t raw_offset =
          Align4(header_offset + sizeof(CachedShapeHeader) + static_cast<std::size_t>(keyfr->height));
      const std::size_t needed = raw_offset + length;

      if (IsTheaterShape) {
        if (needed <= TheaterShapeBufferLength) {
          auto* header = reinterpret_cast<CachedShapeHeader*>(TheaterShapeBuffer.get() + header_offset);
          header->tag = kCachedShapeTag;
          header->draw_flags = -1;
          header->buffer_kind = 1u;
          header->data_offset = static_cast<std::uint32_t>(raw_offset);
          header->data_length = static_cast<std::uint32_t>(length);
          std::memcpy(TheaterShapeBuffer.get() + raw_offset, buffptr, length);
          TheaterShapeBufferUsed = Align4(needed);

          const std::size_t slot = keyfr->y;
          if (slot < KeyFrameSlots.size() && framenumber < KeyFrameSlots[slot].size()) {
            KeyFrameSlots[slot][framenumber] = static_cast<std::uint32_t>(header_offset + 1u);
          }
          return header;
        }
      } else {
        if ((BigShapeBufferLength - BigShapeBufferUsed) < kMinFreeSpaceBeforeGrow) {
          grow_big_buffer(BigShapeBufferLength + kBigShapeBufferGrowSize);
        }
        if (needed > BigShapeBufferLength && !grow_big_buffer(needed)) {
          UseBigShapeBuffer = false;
          return buffptr;
        }

        auto* header = reinterpret_cast<CachedShapeHeader*>(BigShapeBuffer.get() + header_offset);
        header->tag = kCachedShapeTag;
        header->draw_flags = -1;
        header->buffer_kind = 0u;
        header->data_offset = static_cast<std::uint32_t>(raw_offset);
        header->data_length = static_cast<std::uint32_t>(length);
        std::memcpy(BigShapeBuffer.get() + raw_offset, buffptr, length);
        BigShapeBufferUsed = Align4(needed);

        const std::size_t slot = keyfr->y;
        if (slot < KeyFrameSlots.size() && framenumber < KeyFrameSlots[slot].size()) {
          KeyFrameSlots[slot][framenumber] = static_cast<std::uint32_t>(header_offset + 1u);
        }
        return header;
      }
    }

    return buffptr;
  }

  unsigned short currframe = 0;
  if ((frameflags & KF_DELTA) != 0) {
    currframe = static_cast<unsigned short>(offset[1]);
    const char* base =
        static_cast<const char*>(Add_Long_To_Pointer(const_cast<void*>(dataptr),
                                                     (static_cast<unsigned long>(currframe) << 3) +
                                                         sizeof(KeyFrameHeaderType)));
    Mem_Copy(&offset[0], base, static_cast<std::size_t>(kSubframeOffsets) * sizeof(unsigned long));
  }

  unsigned long offcurr = offset[1] & 0x00FFFFFFUL;
  unsigned long offdiff = (offset[0] & 0x00FFFFFFUL) - offcurr;

  const char* key_ptr =
      static_cast<const char*>(Add_Long_To_Pointer(const_cast<void*>(dataptr), offcurr));
  if (has_palette) key_ptr = static_cast<const char*>(Add_Long_To_Pointer(const_cast<char*>(key_ptr), 768L));

  const unsigned long written = LCW_Uncompress(key_ptr, buffptr, buffsize);
  if (written > buffsize) return 0;

  // Apply keydelta to the decoded keyframe.
  Apply_XOR_Delta(buffptr, Add_Long_To_Pointer(const_cast<char*>(key_ptr), static_cast<long>(offdiff)));

  if ((frameflags & KF_DELTA) != 0) {
    // Apply subsequent delta frames up to the requested frame.
    currframe++;
    int subframe = 2;
    while (currframe <= framenumber) {
      offdiff = (offset[subframe] & 0x00FFFFFFUL) - offcurr;
      Apply_XOR_Delta(buffptr, Add_Long_To_Pointer(const_cast<char*>(key_ptr), static_cast<long>(offdiff)));
      currframe++;
      subframe += 2;

      if (subframe >= (kSubframeOffsets - 1) && currframe <= framenumber) {
        const char* more =
            static_cast<const char*>(Add_Long_To_Pointer(const_cast<void*>(dataptr),
                                                         (static_cast<unsigned long>(currframe) << 3) +
                                                             sizeof(KeyFrameHeaderType)));
        Mem_Copy(&offset[0], more, static_cast<std::size_t>(kSubframeOffsets) * sizeof(unsigned long));
        subframe = 0;
      }
    }
  }

  LastFrameLength = static_cast<int>(buffsize);

  if (slots_ready()) {
    const std::size_t length = static_cast<std::size_t>(buffsize);
    const std::size_t header_offset =
        Align4(IsTheaterShape ? TheaterShapeBufferUsed : BigShapeBufferUsed);
    const std::size_t raw_offset =
        Align4(header_offset + sizeof(CachedShapeHeader) + static_cast<std::size_t>(keyfr->height));
    const std::size_t needed = raw_offset + length;

    if (IsTheaterShape) {
      if (needed <= TheaterShapeBufferLength) {
        auto* header = reinterpret_cast<CachedShapeHeader*>(TheaterShapeBuffer.get() + header_offset);
        header->tag = kCachedShapeTag;
        header->draw_flags = -1;
        header->buffer_kind = 1u;
        header->data_offset = static_cast<std::uint32_t>(raw_offset);
        header->data_length = static_cast<std::uint32_t>(length);
        std::memcpy(TheaterShapeBuffer.get() + raw_offset, buffptr, length);
        TheaterShapeBufferUsed = Align4(needed);

        const std::size_t slot = keyfr->y;
        if (slot < KeyFrameSlots.size() && framenumber < KeyFrameSlots[slot].size()) {
          KeyFrameSlots[slot][framenumber] = static_cast<std::uint32_t>(header_offset + 1u);
        }
        return header;
      }
    } else {
      if ((BigShapeBufferLength - BigShapeBufferUsed) < kMinFreeSpaceBeforeGrow) {
        grow_big_buffer(BigShapeBufferLength + kBigShapeBufferGrowSize);
      }
      if (needed > BigShapeBufferLength && !grow_big_buffer(needed)) {
        UseBigShapeBuffer = false;
        return buffptr;
      }

      auto* header = reinterpret_cast<CachedShapeHeader*>(BigShapeBuffer.get() + header_offset);
      header->tag = kCachedShapeTag;
      header->draw_flags = -1;
      header->buffer_kind = 0u;
      header->data_offset = static_cast<std::uint32_t>(raw_offset);
      header->data_length = static_cast<std::uint32_t>(length);
      std::memcpy(BigShapeBuffer.get() + raw_offset, buffptr, length);
      BigShapeBufferUsed = Align4(needed);

      const std::size_t slot = keyfr->y;
      if (slot < KeyFrameSlots.size() && framenumber < KeyFrameSlots[slot].size()) {
        KeyFrameSlots[slot][framenumber] = static_cast<std::uint32_t>(header_offset + 1u);
      }
      return header;
    }
  }

  return buffptr;
}
