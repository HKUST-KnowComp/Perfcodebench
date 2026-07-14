#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  // Unroll by 4 for better ILP
  for (; i + 3 < len; i += 4) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 3]);
    hash *= prime;
  }
  
  // Handle remainder
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
  }
  
  return hash;
}

}  // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  const size_t out_size = static_cast<size_t>(height - 2) * static_cast<size_t>(width - 2);
  std::vector<uint8_t> out(out_size);
  uint64_t hash = 0;
  
  const size_t w = static_cast<size_t>(width);
  const uint8_t* img = image.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* dst = out.data();
    
    for (int y = 1; y < height - 1; ++y) {
      const size_t row_offset = static_cast<size_t>(y) * w;
      const uint8_t* row_prev = img + (row_offset - w);
      const uint8_t* row_curr = img + row_offset;
      const uint8_t* row_next = img + (row_offset + w);
      
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
        const int mag = std::abs(gx) + std::abs(gy);
        
        *dst++ = static_cast<uint8_t>(mag > 255 ? 255 : mag);
      }
    }
    
    hash = checksum_bytes(out.data(), out_size);
  }
  
  return hash;
}