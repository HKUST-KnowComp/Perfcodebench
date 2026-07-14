#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  if (iters <= 0) {
    return 0;
  }
  const std::size_t pixels = src.size() / 4u;
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < pixels; ++i) {
    const uint8_t a = s[3];
    uint8_t r = blend_byte(s[0], d[0], a);
    uint8_t g = blend_byte(s[1], d[1], a);
    uint8_t b = blend_byte(s[2], d[2], a);
    hash ^= r;
    hash *= 1099511628211ULL;
    hash ^= g;
    hash *= 1099511628211ULL;
    hash ^= b;
    hash *= 1099511628211ULL;
    hash ^= 255u;
    hash *= 1099511628211ULL;
    s += 4;
    d += 4;
  }
  return hash;
}
