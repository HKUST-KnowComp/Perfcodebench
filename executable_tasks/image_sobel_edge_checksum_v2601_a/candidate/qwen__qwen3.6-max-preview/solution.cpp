#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  
  uint64_t hash = 0;
  if (iters <= 0) return hash;
  
  const int out_w = width - 2;
  const int out_h = height - 2;
  if (out_w <= 0 || out_h <= 0) {
    return 1469598103934665603ULL;
  }
  
  const size_t w = static_cast<size_t>(width);
  const size_t out_size = static_cast<size_t>(out_w) * static_cast<size_t>(out_h);
  std::vector<uint8_t> out(out_size);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* out_ptr = out.data();
    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* r0 = &image[static_cast<size_t>(y - 1) * w];
      const uint8_t* r1 = &image[static_cast<size_t>(y) * w];
      const uint8_t* r2 = &image[static_cast<size_t>(y + 1) * w];
      
      for (int x = 1; x < width - 1; ++x) {
        int p0 = r0[x-1], p1 = r0[x], p2 = r0[x+1];
        int p3 = r1[x-1], p5 = r1[x+1];
        int p6 = r2[x-1], p7 = r2[x], p8 = r2[x+1];
        
        int gx = -p0 + p2 - 2*p3 + 2*p5 - p6 + p8;
        int gy = p0 + 2*p1 + p2 - p6 - 2*p7 - p8;
        
        int agx = gx < 0 ? -gx : gx;
        int agy = gy < 0 ? -gy : gy;
        int mag = agx + agy;
        
        *out_ptr++ = static_cast<uint8_t>(mag > 255 ? 255 : mag);
      }
    }
    
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_size; ++i) {
      hash ^= static_cast<uint64_t>(out[i]);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}