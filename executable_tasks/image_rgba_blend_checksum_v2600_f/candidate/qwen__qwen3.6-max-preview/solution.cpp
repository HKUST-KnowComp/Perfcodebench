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
  (void)iters;

  const std::size_t pixels = src.size() / 4u;
  uint64_t hash = 1469598103934665603ULL;
  
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  
  for (std::size_t i = 0; i < pixels; ++i) {
    const uint32_t a = s[3];
    const uint32_t inv_a = 255u - a;
    
    const uint32_t r = (s[0] * a + d[0] * inv_a + 127u) / 255u;
    const uint32_t g = (s[1] * a + d[1] * inv_a + 127u) / 255u;
    const uint32_t b = (s[2] * a + d[2] * inv_a + 127u) / 255u;
    
    hash ^= r; hash *= 1099511628211ULL;
    hash ^= g; hash *= 1099511628211ULL;
    hash ^= b; hash *= 1099511628211ULL;
    hash ^= 255u; hash *= 1099511628211ULL;
    
    s += 4;
    d += 4;
  }
  
  return hash;
}
