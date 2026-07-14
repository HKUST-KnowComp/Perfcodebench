#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  
  if (width < 3 || height < 3 || iters <= 0) {
    return 1469598103934665603ULL;
  }
  
  const size_t w = static_cast<size_t>(width);
  const int out_width = width - 2;
  const int out_height = height - 2;
  const size_t out_size = static_cast<size_t>(out_width) * static_cast<size_t>(out_height);
  
  std::vector<uint8_t> out(out_size);
  uint64_t hash = 0;
  
  const uint8_t* __restrict img = image.data();
  uint8_t* __restrict out_ptr = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t out_idx = 0;
    
    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* __restrict row_prev = img + (y - 1) * w;
      const uint8_t* __restrict row_curr = img + y * w;
      const uint8_t* __restrict row_next = img + (y + 1) * w;
      
      for (int x = 1; x < width - 1; ++x) {
        const int p0 = row_prev[x - 1];
        const int p1 = row_prev[x];
        const int p2 = row_prev[x + 1];
        const int p3 = row_curr[x - 1];
        const int p5 = row_curr[x + 1];
        const int p6 = row_next[x - 1];
        const int p7 = row_next[x];
        const int p8 = row_next[x + 1];
        
        const int gx = -p0 + p2 - 2 * p3 + 2 * p5 - p6 + p8;
        const int gy = p0 + 2 * p1 + p2 - p6 - 2 * p7 - p8;
        
        int mag = std::abs(gx) + std::abs(gy);
        if (mag > 255) mag = 255;
        
        const uint8_t val = static_cast<uint8_t>(mag);
        out_ptr[out_idx++] = val;
        
        hash ^= static_cast<uint64_t>(val);
        hash *= 1099511628211ULL;
      }
    }
  }
  
  return hash;
}