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
  const size_t pixels = src.size() / 4;
  if (pixels == 0 || iters <= 0) return 0;

  const uint8_t* s_ptr = src.data();
  const uint8_t* d_ptr = dst.data();

  uint64_t hash = 0;
  const uint64_t fnv_prime = 1099511628211ULL;
  const uint64_t fnv_offset = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    const uint8_t* s = s_ptr;
    const uint8_t* d = d_ptr;
    const uint8_t* s_end = s_ptr + pixels * 4;
    while (s < s_end) {
      const uint32_t a = s[3];
      const uint32_t da = 255u - a;

      uint32_t r_val = s[0] * a + d[0] * da + 127u;
      hash ^= static_cast<uint8_t>(r_val / 255u);
      hash *= fnv_prime;

      uint32_t g_val = s[1] * a + d[1] * da + 127u;
      hash ^= static_cast<uint8_t>(g_val / 255u);
      hash *= fnv_prime;

      uint32_t b_val = s[2] * a + d[2] * da + 127u;
      hash ^= static_cast<uint8_t>(b_val / 255u);
      hash *= fnv_prime;

      hash ^= 255u;
      hash *= fnv_prime;

      s += 4;
      d += 4;
    }
  }
  return hash;
}