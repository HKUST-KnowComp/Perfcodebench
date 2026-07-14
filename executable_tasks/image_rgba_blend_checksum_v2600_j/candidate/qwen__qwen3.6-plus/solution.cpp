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
  
  uint64_t hash = 0;
  if (iters <= 0) return hash;
  
  const size_t pixels = src.size() / 4;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      uint8_t ia = 255 - a;
      
      uint32_t r = (static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * ia + 127) / 255;
      uint32_t g = (static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * ia + 127) / 255;
      uint32_t b = (static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * ia + 127) / 255;
      
      hash ^= r; hash *= FNV_PRIME;
      hash ^= g; hash *= FNV_PRIME;
      hash ^= b; hash *= FNV_PRIME;
      hash ^= 255; hash *= FNV_PRIME;
      
      s += 4;
      d += 4;
    }
  }
  return hash;
}