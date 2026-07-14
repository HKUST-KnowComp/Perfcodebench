#include "interface.h"
#include <cstdint>
#include <cstdlib>
#include <vector>

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  (void)iters;
  if (width < 3 || height < 3) {
    return 1469598103934665603ULL;
  }

  const uint8_t* img = image.data();
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int y = 1; y < height - 1; ++y) {
    const uint8_t* r0 = img + static_cast<size_t>(y - 1) * static_cast<size_t>(width);
    const uint8_t* r1 = img + static_cast<size_t>(y) * static_cast<size_t>(width);
    const uint8_t* r2 = img + static_cast<size_t>(y + 1) * static_cast<size_t>(width);
    
    for (int x = 1; x < width - 1; ++x) {
      int gx = -r0[x-1] + r0[x+1] - 2*r1[x-1] + 2*r1[x+1] - r2[x-1] + r2[x+1];
      int gy = r0[x-1] + 2*r0[x] + r0[x+1] - r2[x-1] - 2*r2[x] - r2[x+1];
      
      int mag = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);
      uint8_t val = (mag > 255) ? 255 : static_cast<uint8_t>(mag);
      
      hash ^= val;
      hash *= FNV_PRIME;
    }
  }
  return hash;
}