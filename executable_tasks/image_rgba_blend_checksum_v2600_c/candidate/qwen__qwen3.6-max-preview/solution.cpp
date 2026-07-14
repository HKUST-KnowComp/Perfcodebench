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
  if (iters <= 0) return 0;

  const size_t n = src.size();
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;

  for (size_t i = 0; i < n; i += 4) {
    uint32_t a = s[i + 3];
    uint32_t inv_a = 255u - a;

    uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[i]) * a + static_cast<uint32_t>(d[i]) * inv_a + 127u) / 255u);
    uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[i + 1]) * a + static_cast<uint32_t>(d[i + 1]) * inv_a + 127u) / 255u);
    uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[i + 2]) * a + static_cast<uint32_t>(d[i + 2]) * inv_a + 127u) / 255u);

    hash ^= r; hash *= prime;
    hash ^= g; hash *= prime;
    hash ^= b; hash *= prime;
    hash ^= 255u; hash *= prime;
  }
  return hash;
}
