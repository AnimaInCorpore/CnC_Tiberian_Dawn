#include "legacy/function.h"

#include <cstdint>
#include <cstring>

namespace {
inline std::uint16_t Read_U16(void const* ptr) {
  std::uint16_t value = 0;
  std::memcpy(&value, ptr, sizeof(value));
  return value;
}

inline std::uint32_t Read_U32(void const* ptr) {
  std::uint32_t value = 0;
  std::memcpy(&value, ptr, sizeof(value));
  return value;
}
}  // namespace

void* Extract_Shape(void const* shape, int index) {
  if (!shape || index < 0) return nullptr;
  const int count = static_cast<int>(Read_U16(shape));
  if (index >= count) return nullptr;

  auto const* bytes = static_cast<std::uint8_t const*>(shape);
  auto const* offsets = reinterpret_cast<std::uint8_t const*>(bytes + sizeof(std::uint16_t));
  const std::uint32_t offset = Read_U32(offsets + static_cast<std::size_t>(index) * sizeof(std::uint32_t));
  return const_cast<std::uint8_t*>(bytes + sizeof(std::uint16_t) + offset);
}
