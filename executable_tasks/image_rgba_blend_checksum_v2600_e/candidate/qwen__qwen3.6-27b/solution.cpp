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
  uint64_t hash = 0;

  const uint8_t* s_ptr = src.data();
  const uint8_t* d_ptr = dst.data();

  for (int iter = 0; iter < iters; ++iter) {
    s_ptr = src.data();
    d_ptr = dst.data();
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < pixels; ++i) {
      uint8_t a = s_ptr[3];
      uint8_t inv_a = 255u - a;
      uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[0]) * a + static_cast<uint32_t>(d_ptr[0]) * inv_a + 127u) / 255u);
      uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[1]) * a + static_cast<uint32_t>(d_ptr[1]) * inv_a + 127u) / 255u);
      uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s_ptr[2]) * a + static_cast<uint32_t>(d_ptr[2]) * inv_a + 127u) / 255u);

      hash ^= r; hash *= 1099511628211ULL;
      hash ^= g; hash *= 1099511628211ULL;
      hash ^= b; hash *= 1099511628211ULL;
      hash ^= 255u; hash *= 1099511628211ULL;

      s_ptr += 4;
      d_ptr += 4;
    }
  }
  return hash;
}