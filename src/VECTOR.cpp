#include "VECTOR.h"

#include <cstring>

namespace {
constexpr int kBitsPerByte = 8;
}

namespace cnc {
namespace detail {

bool GetBooleanBit(unsigned char const* data, int bit) {
  if (!data || bit < 0) {
    return false;
  }
  const int byte_index = bit / kBitsPerByte;
  const int bit_index = bit % kBitsPerByte;
  const unsigned char mask =
      static_cast<unsigned char>(1u << static_cast<unsigned>(bit_index));
  return (data[byte_index] & mask) != 0;
}

void SetBooleanBit(unsigned char* data, int bit, bool value) {
  if (!data || bit < 0) {
    return;
  }
  const int byte_index = bit / kBitsPerByte;
  const int bit_index = bit % kBitsPerByte;
  const unsigned char mask =
      static_cast<unsigned char>(1u << static_cast<unsigned>(bit_index));
  if (value) {
    data[byte_index] |= mask;
  } else {
    data[byte_index] &= static_cast<unsigned char>(~mask);
  }
}

int FirstTrueBit(unsigned char const* data, int bit_count) {
  if (!data || bit_count <= 0) {
    return bit_count;
  }
  for (int index = 0; index < bit_count; ++index) {
    if (GetBooleanBit(data, index)) {
      return index;
    }
  }
  return bit_count;
}

int FirstFalseBit(unsigned char const* data, int bit_count) {
  if (!data || bit_count <= 0) {
    return bit_count;
  }
  for (int index = 0; index < bit_count; ++index) {
    if (!GetBooleanBit(data, index)) {
      return index;
    }
  }
  return bit_count;
}

}  // namespace detail

namespace {
unsigned BytesForBits(int bit_count) {
  if (bit_count <= 0) {
    return 0;
  }
  return static_cast<unsigned>((bit_count + (kBitsPerByte - 1)) /
                               kBitsPerByte);
}
}  // namespace

BooleanVectorClass::BooleanVectorClass(unsigned size, unsigned char* array)
    : BitCount(static_cast<int>(size)),
      Copy(false),
      LastIndex(-1),
      BitArray((size + 7) / 8, array) {}

BooleanVectorClass::BooleanVectorClass(BooleanVectorClass const& vector)
    : BitCount(0), Copy(false), LastIndex(-1), BitArray() {
  *this = vector;
}

BooleanVectorClass& BooleanVectorClass::operator=(
    BooleanVectorClass const& vector) {
  if (this == &vector) {
    return *this;
  }

  Fixup();
  vector.Fixup();

  BitCount = vector.BitCount;
  const unsigned byte_count = BytesForBits(BitCount);
  if (!BitArray.Resize(byte_count)) {
    BitCount = 0;
    LastIndex = -1;
    Copy = false;
    return *this;
  }

  unsigned char* data = Data();
  unsigned char const* other = vector.Data();
  if (byte_count && data && other) {
    std::memcpy(data, other, byte_count);
  }

  LastIndex = -1;
  Copy = false;
  return *this;
}

int BooleanVectorClass::operator==(BooleanVectorClass const& vector) {
  Fixup();
  vector.Fixup();

  if (BitCount != vector.BitCount) {
    return false;
  }
  const unsigned byte_count = BytesForBits(BitCount);
  if (!byte_count) {
    return true;
  }

  unsigned char const* data = Data();
  unsigned char const* other = vector.Data();
  if (!data || !other) {
    return data == other;
  }

  return std::memcmp(data, other, byte_count) == 0;
}

void BooleanVectorClass::Reset() {
  Fixup(-1);
  if (unsigned char* data = Data()) {
    std::memset(data, 0, BitArray.Length());
  }
}

void BooleanVectorClass::Set() {
  Fixup(-1);
  if (unsigned char* data = Data()) {
    std::memset(data, 0xFF, BitArray.Length());
  }
}

void BooleanVectorClass::Clear() {
  Fixup(-1);
  BitCount = 0;
  BitArray.Clear();
  LastIndex = -1;
  Copy = false;
}

int BooleanVectorClass::Resize(unsigned size) {
  Fixup();
  if (!size) {
    Clear();
    return true;
  }

  const int oldsize = BitCount;
  BitCount = static_cast<int>(size);

  const unsigned byte_count = BytesForBits(BitCount);
  const int success = BitArray.Resize(byte_count);
  if (success && oldsize < BitCount) {
    unsigned char* data = Data();
    for (int index = oldsize; index < BitCount; ++index) {
      detail::SetBooleanBit(data, index, false);
    }
  }

  return success;
}

bool BooleanVectorClass::Is_True(int index) const {
  if (index == LastIndex) {
    return Copy;
  }
  return detail::GetBooleanBit(Data(), index);
}

int BooleanVectorClass::First_False() const {
  if (LastIndex != -1) {
    Fixup(-1);
  }
  const int retval = detail::FirstFalseBit(Data(), BitCount);
  if (retval < BitCount) {
    return retval;
  }
  return -1;
}

int BooleanVectorClass::First_True() const {
  if (LastIndex != -1) {
    Fixup(-1);
  }
  const int retval = detail::FirstTrueBit(Data(), BitCount);
  if (retval < BitCount) {
    return retval;
  }
  return -1;
}

void BooleanVectorClass::Fixup(int index) const {
  if (index < 0 || index >= BitCount) {
    index = -1;
  }

  if (index == LastIndex) {
    return;
  }

  unsigned char* data = Data();
  if (LastIndex != -1 && data) {
    detail::SetBooleanBit(data, LastIndex, Copy);
  }

  if (index != -1 && data) {
    Copy = detail::GetBooleanBit(data, index);
  }

  LastIndex = index;
}

unsigned char* BooleanVectorClass::Data() {
  return BitArray.Length() ? &BitArray[0] : nullptr;
}

unsigned char* BooleanVectorClass::Data() const {
  return BitArray.Length()
             ? const_cast<unsigned char*>(&BitArray[0])
             : nullptr;
}

}  // namespace cnc
