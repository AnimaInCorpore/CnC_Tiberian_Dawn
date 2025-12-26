#include "vqa_decoder.h"

#include "legacy/ccfile.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

namespace {

class FileReader {
 public:
  explicit FileReader(const std::string& filename) : file_(filename.c_str()) {}

  bool Open() { return file_.Open() != 0; }
  void Close() { file_.Close(); }
  bool IsOpen() const { return file_.Is_Open() != 0; }
  long Size() { return file_.Size(); }

  long Tell() { return file_.Seek(0, SEEK_CUR); }
  bool Seek(long pos) { return file_.Seek(pos, SEEK_SET) >= 0; }

  bool ReadExact(void* dest, std::size_t n) {
    if (!dest && n) return false;
    long r = file_.Read(dest, static_cast<long>(n));
    return r == static_cast<long>(n);
  }

  std::uint8_t ReadU8(bool* ok) {
    std::uint8_t v = 0;
    if (!ReadExact(&v, 1)) {
      if (ok) *ok = false;
      return 0;
    }
    return v;
  }

  std::uint16_t ReadU16LE(bool* ok) {
    std::uint8_t b[2]{};
    if (!ReadExact(b, sizeof(b))) {
      if (ok) *ok = false;
      return 0;
    }
    return static_cast<std::uint16_t>(b[0] | (static_cast<std::uint16_t>(b[1]) << 8));
  }

  std::uint32_t ReadU32LE(bool* ok) {
    std::uint8_t b[4]{};
    if (!ReadExact(b, sizeof(b))) {
      if (ok) *ok = false;
      return 0;
    }
    return static_cast<std::uint32_t>(b[0]) | (static_cast<std::uint32_t>(b[1]) << 8) |
           (static_cast<std::uint32_t>(b[2]) << 16) | (static_cast<std::uint32_t>(b[3]) << 24);
  }

  std::uint32_t ReadU32BE(bool* ok) {
    std::uint8_t b[4]{};
    if (!ReadExact(b, sizeof(b))) {
      if (ok) *ok = false;
      return 0;
    }
    return (static_cast<std::uint32_t>(b[0]) << 24) | (static_cast<std::uint32_t>(b[1]) << 16) |
           (static_cast<std::uint32_t>(b[2]) << 8) | static_cast<std::uint32_t>(b[3]);
  }

  bool PeekIsNullByte() {
    const long at = Tell();
    if (at < 0) return false;
    std::uint8_t b = 0;
    if (!ReadExact(&b, 1)) return false;
    Seek(at);
    return b == 0;
  }

  bool ReadTag(char out[5]) {
    if (!ReadExact(out, 4)) return false;
    out[4] = '\0';
    return true;
  }

 private:
  CCFileClass file_;
};

void LCW_ReplicatePrevious(std::uint8_t* dest, int dest_index, int src_index, int count) {
  if (src_index > dest_index) return;
  for (int i = 0; i < count; ++i) {
    if (dest_index - src_index == 1) {
      dest[dest_index + i] = dest[dest_index - 1];
    } else {
      dest[dest_index + i] = dest[src_index + i];
    }
  }
}

int LCW_DecodeInto(const std::uint8_t* src, int src_len, std::uint8_t* dest, int dest_len, int src_offset = 0,
                   bool reverse = false) {
  int si = src_offset;
  int di = 0;
  while (true) {
    if (si >= src_len) return di;
    const std::uint8_t i = src[si++];
    if ((i & 0x80) == 0) {
      if (si >= src_len) return di;
      const std::uint8_t second = src[si++];
      const int count = ((i & 0x70) >> 4) + 3;
      const int rpos = ((i & 0x0f) << 8) + second;
      if (di + count > dest_len) return di;
      LCW_ReplicatePrevious(dest, di, di - rpos, count);
      di += count;
    } else if ((i & 0x40) == 0) {
      const int count = i & 0x3f;
      if (count == 0) return di;
      if (si + count > src_len) return di;
      const int to_copy = std::min(count, dest_len - di);
      std::memcpy(dest + di, src + si, static_cast<std::size_t>(to_copy));
      si += count;
      di += to_copy;
      if (to_copy != count) return di;
    } else {
      const int count3 = i & 0x3f;
      if (count3 == 0x3e) {
        if (si + 3 > src_len) return di;
        const int count = static_cast<int>(src[si] | (src[si + 1] << 8));
        si += 2;
        const std::uint8_t color = src[si++];
        const int end = std::min(dest_len, di + count);
        while (di < end) dest[di++] = color;
        if (di != end) return di;
      } else {
        int count = 0;
        if (count3 == 0x3f) {
          if (si + 2 > src_len) return di;
          count = static_cast<int>(src[si] | (src[si + 1] << 8));
          si += 2;
        } else {
          count = count3 + 3;
        }

        if (si + 2 > src_len) return di;
        const int word = static_cast<int>(src[si] | (src[si + 1] << 8));
        si += 2;
        const int src_index = reverse ? (di - word) : word;
        if (src_index >= di) return di;
        const int end = std::min(dest_len, di + count);
        int cur = src_index;
        while (di < end) dest[di++] = dest[cur++];
      }
    }
  }
}

struct Vec2i {
  int x = 0;
  int y = 0;
};

}  // namespace

struct VqaDecoder::Impl {
  std::unique_ptr<FileReader> file;
  std::vector<std::uint32_t> offsets;

  std::uint16_t frame_count = 0;
  std::uint8_t frame_rate = 0;
  std::uint16_t width = 0;
  std::uint16_t height = 0;
  std::uint8_t block_w = 0;
  std::uint8_t block_h = 0;
  std::uint16_t num_colors = 0;
  std::uint16_t audio_rate = 0;
  std::uint8_t audio_channels = 0;
  std::uint8_t audio_bits = 0;
  std::uint16_t flags = 0;

  Vec2i blocks{};
  std::vector<std::uint8_t> cbf;
  std::vector<std::uint8_t> cbp;
  std::vector<std::uint8_t> orig_data;
  std::array<std::uint8_t, 256 * 3> palette{};  // 6-bit RGB triplets

  int chunk_buffer_parts = 0;
  int current_chunk_buffer = 0;
  int chunk_buffer_offset = 0;
  bool cbp_is_compressed = false;

  std::vector<std::uint8_t> scratch;

  bool ParseHeader();
  bool LoadFrame(std::uint16_t frame_index);
  bool DecodeVQFR(long end_offset, bool vqfl);
  void DecodeFrameIndices(std::vector<std::uint8_t>& out_indices) const;
};

VqaDecoder::VqaDecoder() : impl_(new Impl) {}
VqaDecoder::~VqaDecoder() { delete impl_; }

VqaDecoder::VqaDecoder(VqaDecoder&& other) noexcept : impl_(other.impl_) { other.impl_ = nullptr; }
VqaDecoder& VqaDecoder::operator=(VqaDecoder&& other) noexcept {
  if (this == &other) return *this;
  delete impl_;
  impl_ = other.impl_;
  other.impl_ = nullptr;
  return *this;
}

bool VqaDecoder::Open(const std::string& filename) {
  if (!impl_) return false;
  Close();
  impl_->file = std::make_unique<FileReader>(filename);
  if (!impl_->file->Open()) {
    impl_->file.reset();
    return false;
  }
  if (!impl_->ParseHeader()) {
    Close();
    return false;
  }
  return true;
}

void VqaDecoder::Close() {
  if (!impl_) return;
  if (impl_->file) impl_->file->Close();
  impl_->file.reset();
  impl_->offsets.clear();
  impl_->cbf.clear();
  impl_->cbp.clear();
  impl_->orig_data.clear();
  impl_->scratch.clear();
  impl_->frame_count = 0;
  impl_->frame_rate = 0;
  impl_->width = 0;
  impl_->height = 0;
}

bool VqaDecoder::Is_Open() const { return impl_ && impl_->file && impl_->file->IsOpen(); }
std::uint16_t VqaDecoder::Frame_Count() const { return impl_ ? impl_->frame_count : 0; }
std::uint8_t VqaDecoder::Frame_Rate() const { return impl_ ? impl_->frame_rate : 0; }
std::uint16_t VqaDecoder::Width() const { return impl_ ? impl_->width : 0; }
std::uint16_t VqaDecoder::Height() const { return impl_ ? impl_->height : 0; }

bool VqaDecoder::Decode_Frame(std::uint16_t frame_index, Frame& out) {
  if (!Is_Open()) return false;
  if (frame_index >= Frame_Count()) return false;
  if (!impl_->LoadFrame(frame_index)) return false;

  out.width = impl_->width;
  out.height = impl_->height;
  out.palette = impl_->palette;
  out.indices.resize(static_cast<std::size_t>(impl_->width) * static_cast<std::size_t>(impl_->height));
  impl_->DecodeFrameIndices(out.indices);
  return true;
}

bool VqaDecoder::Impl::ParseHeader() {
  bool ok = true;
  char tag[5]{};
  if (!file->ReadTag(tag) || std::string(tag) != "FORM") return false;
  (void)file->ReadU32BE(&ok);  // length (unused)
  if (!ok) return false;

  char tag2[9]{};
  if (!file->ReadExact(tag2, 8)) return false;
  tag2[8] = '\0';
  if (std::string(tag2) != "WVQAVQHD") return false;
  (void)file->ReadU32BE(&ok);  // header length
  if (!ok) return false;

  (void)file->ReadU16LE(&ok);  // version
  flags = file->ReadU16LE(&ok);
  frame_count = file->ReadU16LE(&ok);
  width = file->ReadU16LE(&ok);
  height = file->ReadU16LE(&ok);
  block_w = file->ReadU8(&ok);
  block_h = file->ReadU8(&ok);
  frame_rate = file->ReadU8(&ok);
  chunk_buffer_parts = file->ReadU8(&ok);
  num_colors = file->ReadU16LE(&ok);
  (void)file->ReadU16LE(&ok);  // maxBlocks
  (void)file->ReadU16LE(&ok);  // unknown1
  (void)file->ReadU32LE(&ok);  // unknown2
  audio_rate = file->ReadU16LE(&ok);
  audio_channels = file->ReadU8(&ok);
  audio_bits = file->ReadU8(&ok);
  (void)file->ReadU32LE(&ok);
  (void)file->ReadU16LE(&ok);
  (void)file->ReadU32LE(&ok);  // maxCbfzSize (unreliable)
  (void)file->ReadU32LE(&ok);

  if (!ok) return false;
  if ((flags & 0x10u) != 0) return false;  // HQ (TS+) not supported here.
  if (block_w == 0 || block_h == 0) return false;
  if (width % block_w != 0 || height % block_h != 0) return false;

  blocks = {static_cast<int>(width / block_w), static_cast<int>(height / block_h)};
  cbf.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  cbp.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  orig_data.resize(static_cast<std::size_t>(2 * blocks.x * blocks.y));
  scratch.resize(256000);
  std::fill(palette.begin(), palette.end(), 0);

  // Scan for FINF.
  while (true) {
    if (!file->ReadTag(tag)) return false;
    if (std::string(tag) == "FINF") break;

    // "??F?" tags denote file tag entries (per OpenRA).
    if (tag[3] == 'F') {
      const std::uint32_t jmp = file->ReadU32BE(&ok);
      if (!ok) return false;
      const long cur = file->Tell();
      if (cur < 0) return false;
      if (!file->Seek(cur + static_cast<long>(jmp))) return false;
      continue;
    }
    return false;
  }

  (void)file->ReadU16LE(&ok);  // length
  (void)file->ReadU16LE(&ok);
  if (!ok) return false;

  offsets.resize(frame_count);
  for (std::uint16_t i = 0; i < frame_count; ++i) {
    std::uint32_t off = file->ReadU32LE(&ok);
    if (!ok) return false;
    if (off > 0x40000000u) off -= 0x40000000u;
    off <<= 1;
    offsets[i] = off;
  }

  current_chunk_buffer = 0;
  chunk_buffer_offset = 0;
  cbp_is_compressed = false;
  return true;
}

bool VqaDecoder::Impl::LoadFrame(std::uint16_t frame_index) {
  if (frame_index >= frame_count) return false;
  const long file_size = file->Size();
  if (file_size <= 0) return false;

  const std::uint32_t start = offsets[frame_index];
  const std::uint32_t end_u32 =
      (frame_index + 1 < frame_count) ? offsets[frame_index + 1] : static_cast<std::uint32_t>(file_size);
  if (start >= end_u32) return false;
  if (!file->Seek(static_cast<long>(start))) return false;

  long end = static_cast<long>(end_u32);
  bool ok = true;
  while (file->Tell() < end) {
    char type[5]{};
    if (!file->ReadTag(type)) return false;

    // Skip SN2J preamble chunks if present.
    if (std::string(type) == "SN2J") {
      const std::uint32_t jmp = file->ReadU32BE(&ok);
      if (!ok) return false;
      const long cur = file->Tell();
      if (!file->Seek(cur + static_cast<long>(jmp))) return false;
      if (!file->ReadTag(type)) return false;
    }

    const std::uint32_t length = file->ReadU32BE(&ok);
    if (!ok) return false;

    const long after_len = file->Tell();
    if (after_len < 0) return false;

    const std::string t(type);
    if (t == "VQFR") {
      if (!DecodeVQFR(end, false)) return false;
    } else if (t == "VQFL") {
      if (!DecodeVQFR(end, true)) return false;
    } else if (t.size() == 4 && t[0] == '\0' && t[1] == 'V' && t[2] == 'Q' && t[3] == 'F') {
      (void)file->ReadU8(&ok);
      if (!ok) return false;
      if (!DecodeVQFR(end, false)) return false;
    } else {
      // Skip unknown chunk payload.
      if (!file->Seek(after_len + static_cast<long>(length))) return false;
    }

    // Chunk alignment padding.
    if (file->PeekIsNullByte()) {
      (void)file->ReadU8(&ok);
      if (!ok) return false;
    }
  }
  return true;
}

bool VqaDecoder::Impl::DecodeVQFR(long /*end_offset*/, bool vqfl) {
  // Apply pending CBP tables when complete.
  if (chunk_buffer_parts != 0 && current_chunk_buffer == chunk_buffer_parts) {
    if (!cbp_is_compressed) {
      cbf = cbp;
    } else {
      std::fill(cbf.begin(), cbf.end(), 0);
      (void)LCW_DecodeInto(cbp.data(), static_cast<int>(cbp.size()), cbf.data(), static_cast<int>(cbf.size()));
    }
    chunk_buffer_offset = 0;
    current_chunk_buffer = 0;
  }

  bool ok = true;
  while (true) {
    // Subchunks are aligned on even bytes and may be prefixed by a single null.
    if (file->PeekIsNullByte()) {
      (void)file->ReadU8(&ok);
      if (!ok) return false;
    }

    char sub[5]{};
    if (!file->ReadTag(sub)) return false;
    const std::string subtype(sub);
    const std::uint32_t sub_len = file->ReadU32BE(&ok);
    if (!ok) return false;

    if (sub_len > scratch.size()) {
      scratch.resize(sub_len);
    }

    if (subtype == "CBF0") {
      cbf.resize(static_cast<std::size_t>(sub_len));
      if (!file->ReadExact(cbf.data(), sub_len)) return false;
      if (vqfl) return true;
    } else if (subtype == "CBFZ") {
      if (!file->ReadExact(scratch.data(), sub_len)) return false;
      std::fill(cbf.begin(), cbf.end(), 0);
      (void)LCW_DecodeInto(scratch.data(), static_cast<int>(sub_len), cbf.data(), static_cast<int>(cbf.size()),
                          (scratch[0] == 0) ? 1 : 0, scratch[0] == 0);
      if (vqfl) return true;
    } else if (subtype == "CBP0" || subtype == "CBPZ") {
      if (chunk_buffer_offset + static_cast<int>(sub_len) > static_cast<int>(cbp.size())) {
        // Defensive: clamp to storage size.
        const int remaining = static_cast<int>(cbp.size()) - chunk_buffer_offset;
        if (remaining > 0) {
          if (!file->ReadExact(cbp.data() + chunk_buffer_offset, static_cast<std::size_t>(remaining))) return false;
          chunk_buffer_offset += remaining;
        }
        // Skip leftover.
        const long cur = file->Tell();
        if (!file->Seek(cur + static_cast<long>(sub_len - static_cast<std::uint32_t>(std::max(remaining, 0))))) {
          return false;
        }
      } else {
        if (!file->ReadExact(cbp.data() + chunk_buffer_offset, sub_len)) return false;
        chunk_buffer_offset += static_cast<int>(sub_len);
      }
      current_chunk_buffer++;
      cbp_is_compressed = subtype == "CBPZ";
    } else if (subtype == "CPL0") {
      const int colors = std::min<int>(num_colors, 256);
      for (int i = 0; i < colors; ++i) {
        const std::uint8_t r = file->ReadU8(&ok);
        const std::uint8_t g = file->ReadU8(&ok);
        const std::uint8_t b = file->ReadU8(&ok);
        if (!ok) return false;
        palette[static_cast<std::size_t>(i) * 3 + 0] = r;
        palette[static_cast<std::size_t>(i) * 3 + 1] = g;
        palette[static_cast<std::size_t>(i) * 3 + 2] = b;
      }
      // Skip any extra palette bytes if present.
      const std::uint32_t consumed = static_cast<std::uint32_t>(colors) * 3u;
      if (sub_len > consumed) {
        const long cur = file->Tell();
        if (!file->Seek(cur + static_cast<long>(sub_len - consumed))) return false;
      }
    } else if (subtype == "VPTZ") {
      if (!file->ReadExact(scratch.data(), sub_len)) return false;
      std::fill(orig_data.begin(), orig_data.end(), 0);
      (void)LCW_DecodeInto(scratch.data(), static_cast<int>(sub_len), orig_data.data(), static_cast<int>(orig_data.size()));
      return true;
    } else if (subtype == "VPRZ") {
      if (!file->ReadExact(scratch.data(), sub_len)) return false;
      std::fill(orig_data.begin(), orig_data.end(), 0);
      (void)LCW_DecodeInto(scratch.data(), static_cast<int>(sub_len), orig_data.data(), static_cast<int>(orig_data.size()),
                          (scratch[0] == 0) ? 1 : 0, scratch[0] == 0);
      return true;
    } else if (subtype == "VPTR") {
      std::fill(orig_data.begin(), orig_data.end(), 0);
      const std::uint32_t to_read = std::min<std::uint32_t>(sub_len, static_cast<std::uint32_t>(orig_data.size()));
      if (!file->ReadExact(orig_data.data(), to_read)) return false;
      if (sub_len > to_read) {
        const long cur = file->Tell();
        if (!file->Seek(cur + static_cast<long>(sub_len - to_read))) return false;
      }
      return true;
    } else {
      // Unknown/unsupported subchunk: skip payload and keep scanning.
      const long cur = file->Tell();
      if (!file->Seek(cur + static_cast<long>(sub_len))) return false;
    }
  }
}

void VqaDecoder::Impl::DecodeFrameIndices(std::vector<std::uint8_t>& out_indices) const {
  if (out_indices.size() != static_cast<std::size_t>(width) * static_cast<std::size_t>(height)) return;
  if (blocks.x <= 0 || blocks.y <= 0) return;

  const int blocks_x = blocks.x;
  const int blocks_y = blocks.y;

  for (int by = 0; by < blocks_y; ++by) {
    for (int bx = 0; bx < blocks_x; ++bx) {
      const std::uint8_t px = orig_data[static_cast<std::size_t>(bx + by * blocks_x)];
      const std::uint8_t mod = orig_data[static_cast<std::size_t>(bx + (by + blocks_y) * blocks_x)];
      for (int j = 0; j < block_h; ++j) {
        for (int i = 0; i < block_w; ++i) {
          const int cbfi = (static_cast<int>(mod) * 256 + static_cast<int>(px)) * 8 + j * block_w + i;
          const std::uint8_t color_index = (mod == 0x0f) ? px : cbf[static_cast<std::size_t>(cbfi)];
          const int x = bx * block_w + i;
          const int y = by * block_h + j;
          out_indices[static_cast<std::size_t>(y) * width + static_cast<std::size_t>(x)] = color_index;
        }
      }
    }
  }
}

