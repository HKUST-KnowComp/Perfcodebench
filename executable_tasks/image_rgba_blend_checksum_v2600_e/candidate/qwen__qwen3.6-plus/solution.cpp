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
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (std::size_t i = 0; i < pixels; ++i) {
      uint8_t a = s[3];
      uint8_t inv_a = 255 - a;
      
      uint8_t r = static_cast<uint8_t>((static_cast<uint32_t>(s[0]) * a + static_cast<uint32_t>(d[0]) * inv_a + 127) / 255);
      uint8_t g = static_cast<uint8_t>((static_cast<uint32_t>(s[1]) * a + static_cast<uint32_t>(d[1]) * inv_a + 127) / 255);
      uint8_t b = static_cast<uint8_t>((static_cast<uint32_t>(s[2]) * a + static_cast<uint32_t>(d[2]) * inv_a + 127) / 255);
      
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