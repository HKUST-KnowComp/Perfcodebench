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
  
  const size_t n = src.size();
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;
    for (size_t i = 0; i < n; i += 4) {
      uint8_t a = s[i + 3];
      uint32_t ia = 255u - a;
      
      uint32_t r_num = (uint32_t)s[i] * a + (uint32_t)d[i] * ia + 127u;
      uint32_t g_num = (uint32_t)s[i + 1] * a + (uint32_t)d[i + 1] * ia + 127u;
      uint32_t b_num = (uint32_t)s[i + 2] * a + (uint32_t)d[i + 2] * ia + 127u;
      
      uint8_t r = r_num / 255u;
      uint8_t g = g_num / 255u;
      uint8_t b = b_num / 255u;
      
      h ^= r; h *= FNV_PRIME;
      h ^= g; h *= FNV_PRIME;
      h ^= b; h *= FNV_PRIME;
      h ^= 255u; h *= FNV_PRIME;
    }
    hash = h;
  }
  
  return hash;
}
