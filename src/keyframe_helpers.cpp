#include "legacy/function.h"

namespace {

static bool UseBigShapeBuffer = false;
static bool OriginalUseBigShapeBuffer = false;
static int LastFrameLength = 0;

struct KeyFrameHeaderType {
  unsigned short frames;
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  unsigned short largest_frame_size;
  short flags;
};

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

void Disable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = false;
}

void Enable_Uncompressed_Shapes(void) {
  UseBigShapeBuffer = OriginalUseBigShapeBuffer;
}

void* Get_Shape_Header_Data(void* ptr) {
  return ptr;
}

int Get_Last_Frame_Length(void) {
  return LastFrameLength;
}

unsigned long Build_Frame(void const* dataptr, unsigned short framenumber, void* buffptr) {
  LastFrameLength = 0;
  if (!dataptr || !buffptr) return 0;

  const auto* keyfr = static_cast<const KeyFrameHeaderType*>(dataptr);
  if (framenumber >= keyfr->frames) return 0;

  const unsigned long buffsize =
      static_cast<unsigned long>(keyfr->width) * static_cast<unsigned long>(keyfr->height);
  if (buffsize == 0) return 0;

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
    return reinterpret_cast<unsigned long>(buffptr);
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
  return reinterpret_cast<unsigned long>(buffptr);
}
