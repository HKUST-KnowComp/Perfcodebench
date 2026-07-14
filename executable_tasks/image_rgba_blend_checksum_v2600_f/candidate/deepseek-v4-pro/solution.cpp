#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
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

  const std::size_t pixels = src.size() / 4u;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* s = src.data();
    const uint8_t* d = dst.data();

    for (std::size_t i = 0; i < pixels; ++i) {
      const uint8_t a = s[3];

      // Blend R
      uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[0]) * a +
                                        static_cast<uint32_t>(d[0]) * (255u - a) + 127u) / 255u);
      h ^= static_cast<uint64_t>(r);
      h *= 1099511628211ULL;

      // Blend G
      uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[1]) * a +
                                        static_cast<uint32_t>(d[1]) * (255u - a) + 127u) / 255u);
      h ^= static_cast<uint64_t>(g);
      h *= 1099511628211ULL;

      // Blend B
      uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[2]) * a +
                                        static_cast<uint32_t>(d[2]) * (255u - a) + 127u) / 255u);
      h ^= static_cast<uint64_t>(b);
      h *= 1099511628211ULL;

      // Alpha is always 255
      h ^= 255ULL;
      h *= 1099511628211ULL;

      s += 4;
      d += 4;
    }

    hash = h;  // keep hash of final iteration
  }

  return hash;
}
