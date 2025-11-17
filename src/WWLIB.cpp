#include <cmath>
#include <cstdint>
#include <limits>

namespace {
constexpr double kTau = 6.28318530717958647692;
constexpr char kEmptyString[] = "";
}  // namespace

int Desired_Facing256(int x1, int y1, int x2, int y2) {
  const int dx = x2 - x1;
  const int dy = y2 - y1;
  if (dx == 0 && dy == 0) {
    return 0;
  }

  double angle = std::atan2(static_cast<double>(dy), static_cast<double>(dx));
  angle /= kTau;
  if (angle < 0.0) {
    angle += 1.0;
  }

  const int mask = static_cast<int>(std::numeric_limits<std::uint8_t>::max());
  const int value = static_cast<int>(std::lround(angle * 256.0)) & mask;
  return value;
}

int Desired_Facing8(int x1, int y1, int x2, int y2) {
  const int facing256 = Desired_Facing256(x1, y1, x2, y2);
  return static_cast<int>(((facing256 + 16) & 0xFF) >> 5);
}

char const* Extract_String(char const* table, int index) {
  if (!table || index < 0) {
    return kEmptyString;
  }

  const char* current = table;
  while (index > 0 && *current) {
    while (*current++) {
    }
    --index;
  }

  if (!*current) {
    return kEmptyString;
  }
  return current;
}
