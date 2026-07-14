#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  const uint32_t sa = static_cast<uint32_t>(s) * static_cast<uint32_t>(a);
  const uint32_t da = static_cast<uint32_t>(d) * static_cast<uint32_t>(255u - a);
  return static_cast<uint8_t>((sa + da + 127u) / 255u);
}

static inline uint64_t fnv1a_update(uint64_t hash, uint8_t b) {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
  return hash;
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

  const std::size_t n = src.size();
  if (n == 0 || iters <= 0) return 0;

  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; i += 4) {
      const uint8_t a = s[i + 3];

      const uint8_t r = blend_byte_fast(s[i], d[i], a);
      const uint8_t g = blend_byte_fast(s[i + 1], d[i + 1], a);
      const uint8_t b = blend_byte_fast(s[i + 2], d[i + 2], a);

      h = fnv1a_update(h, r);
      h = fnv1a_update(h, g);
      h = fnv1a_update(h, b);
      h = fnv1a_update(h, 255u);
    }

    hash = h;
  }

  return hash;
}
