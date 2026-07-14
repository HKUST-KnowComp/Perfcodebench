#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * static_cast<uint32_t>(a) +
                               static_cast<uint32_t>(d) * static_cast<uint32_t>(255u - a) +
                               127u) / 255u);
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
  if (n == 0 || iters <= 0) {
    return 0;
  }

  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; i += 4) {
      const uint8_t a = s[i + 3];
      const uint32_t ia = 255u - static_cast<uint32_t>(a);
      const uint32_t au = static_cast<uint32_t>(a);

      const uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[i]) * au +
                                              static_cast<uint32_t>(d[i]) * ia + 127u) / 255u);
      const uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[i + 1]) * au +
                                              static_cast<uint32_t>(d[i + 1]) * ia + 127u) / 255u);
      const uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[i + 2]) * au +
                                              static_cast<uint32_t>(d[i + 2]) * ia + 127u) / 255u);

      h ^= static_cast<uint64_t>(r);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(g);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b);
      h *= 1099511628211ULL;
      h ^= 255ULL;
      h *= 1099511628211ULL;
    }

    hash = h;
  }

  return hash;
}
