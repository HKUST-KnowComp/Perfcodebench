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
  const uint8_t* s_ptr = src.data();
  const uint8_t* d_ptr = dst.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* s = s_ptr;
    const uint8_t* d = d_ptr;
    for (std::size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      uint32_t inv_a = 255u - a;
      
      uint32_t r_num = static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inv_a + 127u;
      uint32_t g_num = static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inv_a + 127u;
      uint32_t b_num = static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inv_a + 127u;
      
      h ^= static_cast<uint64_t>(r_num / 255u); h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(g_num / 255u); h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(b_num / 255u); h *= 1099511628211ULL;
      h ^= 255u; h *= 1099511628211ULL;
      
      s += 4;
      d += 4;
    }
    hash = h;
  }
  return hash;
}