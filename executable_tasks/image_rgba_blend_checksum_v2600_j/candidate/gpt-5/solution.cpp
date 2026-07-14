#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;

  const std::size_t pixels = src.size() / 4u;
  if (pixels == 0) {
    // Hash of an empty output image for each iteration is the offset basis; returning it is fine.
    return 1469598103934665603ULL;
  }

  const uint8_t* sp = src.data();
  const uint8_t* dp = dst.data();

  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;

    const uint8_t* s = sp;
    const uint8_t* d = dp;

    std::size_t p = 0;
    const std::size_t limit4 = (pixels / 4u) * 4u; // unroll by 4 pixels

    for (; p < limit4; p += 4u) {
      // Process 4 pixels unrolled
      for (int k = 0; k < 4; ++k) {
        const uint32_t a   = s[3];
        const uint32_t inv = 255u - a;

        const uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inv + 127u) / 255u);
        const uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inv + 127u) / 255u);
        const uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inv + 127u) / 255u);

        h ^= r; h *= FNV_PRIME;
        h ^= g; h *= FNV_PRIME;
        h ^= b; h *= FNV_PRIME;
        h ^= static_cast<uint8_t>(255u); h *= FNV_PRIME;

        s += 4;
        d += 4;
      }
    }

    // Remainder pixels
    for (; p < pixels; ++p) {
      const uint32_t a   = s[3];
      const uint32_t inv = 255u - a;

      const uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inv + 127u) / 255u);
      const uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inv + 127u) / 255u);
      const uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inv + 127u) / 255u);

      h ^= r; h *= FNV_PRIME;
      h ^= g; h *= FNV_PRIME;
      h ^= b; h *= FNV_PRIME;
      h ^= static_cast<uint8_t>(255u); h *= FNV_PRIME;

      s += 4;
      d += 4;
    }

    hash = h; // keep last iteration's hash, matching baseline behavior
  }

  return hash;
}
