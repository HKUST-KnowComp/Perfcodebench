#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * static_cast<uint32_t>(a) +
                               static_cast<uint32_t>(d) * static_cast<uint32_t>(255u - a) +
                               127u) /
                              255u);
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
  if (n == 0 || iters <= 0) {
    return 0;
  }

  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    const std::size_t limit = n & ~std::size_t(15);
    std::size_t i = 0;
    for (; i < limit; i += 16) {
      const uint8_t a0 = s[i + 3];
      const uint8_t a1 = s[i + 7];
      const uint8_t a2 = s[i + 11];
      const uint8_t a3 = s[i + 15];

      const uint8_t b0 = blend_byte(s[i + 0], d[i + 0], a0);
      const uint8_t b1 = blend_byte(s[i + 1], d[i + 1], a0);
      const uint8_t b2 = blend_byte(s[i + 2], d[i + 2], a0);
      h ^= static_cast<uint64_t>(b0);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b1);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b2);
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;

      const uint8_t b4 = blend_byte(s[i + 4], d[i + 4], a1);
      const uint8_t b5 = blend_byte(s[i + 5], d[i + 5], a1);
      const uint8_t b6 = blend_byte(s[i + 6], d[i + 6], a1);
      h ^= static_cast<uint64_t>(b4);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b5);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b6);
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;

      const uint8_t b8 = blend_byte(s[i + 8], d[i + 8], a2);
      const uint8_t b9 = blend_byte(s[i + 9], d[i + 9], a2);
      const uint8_t b10 = blend_byte(s[i + 10], d[i + 10], a2);
      h ^= static_cast<uint64_t>(b8);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b9);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b10);
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;

      const uint8_t b12 = blend_byte(s[i + 12], d[i + 12], a3);
      const uint8_t b13 = blend_byte(s[i + 13], d[i + 13], a3);
      const uint8_t b14 = blend_byte(s[i + 14], d[i + 14], a3);
      h ^= static_cast<uint64_t>(b12);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b13);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b14);
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;
    }

    for (; i < n; i += 4) {
      const uint8_t a = s[i + 3];
      h ^= static_cast<uint64_t>(blend_byte(s[i + 0], d[i + 0], a));
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(blend_byte(s[i + 1], d[i + 1], a));
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(blend_byte(s[i + 2], d[i + 2], a));
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;
    }

    hash = h;
  }

  return hash;
}
