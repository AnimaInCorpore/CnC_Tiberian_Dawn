#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>

// Minimal Westwood VQA (C&C/RA era) decoder.
// - Supports the 8-bit (non-HQ) VQA variant used by Tiberian Dawn assets.
// - Decodes frames to an 8-bit index buffer and exposes the active 6-bit palette (768 bytes).
// - Uses LCW/Format80 decompression for VQA subchunks.
class VqaDecoder {
 public:
  struct Frame {
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::vector<std::uint8_t> indices;  // width*height
    std::array<std::uint8_t, 256 * 3> palette{};  // 6-bit RGB triplets (0..63)
  };

  VqaDecoder();
  ~VqaDecoder();

  VqaDecoder(const VqaDecoder&) = delete;
  VqaDecoder& operator=(const VqaDecoder&) = delete;
  VqaDecoder(VqaDecoder&&) noexcept;
  VqaDecoder& operator=(VqaDecoder&&) noexcept;

  bool Open(const std::string& filename);
  void Close();

  bool Is_Open() const;
  std::uint16_t Frame_Count() const;
  std::uint8_t Frame_Rate() const;
  std::uint16_t Width() const;
  std::uint16_t Height() const;

  bool Decode_Frame(std::uint16_t frame_index, Frame& out);

 private:
  struct Impl;
  Impl* impl_ = nullptr;
};
