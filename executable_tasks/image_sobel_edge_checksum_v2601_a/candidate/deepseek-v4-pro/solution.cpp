#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  if (iters == 0) return 0;

  const uint8_t* img = image.data();
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t mul = 1099511628211ULL;

  for (int y = 1; y < height - 1; ++y) {
    const uint8_t* row_above = img + (y - 1) * width;
    const uint8_t* row_curr  = img + y * width;
    const uint8_t* row_below = img + (y + 1) * width;
    const uint8_t* ptr_above = row_above + 1;
    const uint8_t* ptr_curr  = row_curr + 1;
    const uint8_t* ptr_below = row_below + 1;

    for (int x = 1; x < width - 1; ++x) {
      int p0 = ptr_above[-1];
      int p1 = ptr_above[0];
      int p2 = ptr_above[1];
      int p3 = ptr_curr[-1];
      int p4 = ptr_curr[0];
      int p5 = ptr_curr[1];
      int p6 = ptr_below[-1];
      int p7 = ptr_below[0];
      int p8 = ptr_below[1];

      int gx = -p0 + p2 - 2 * p3 + 2 * p5 - p6 + p8;
      int gy = p0 + 2 * p1 + p2 - p6 - 2 * p7 - p8;
      int mag = std::abs(gx) + std::abs(gy);
      uint8_t pixel = static_cast<uint8_t>(mag > 255 ? 255 : mag);

      hash ^= static_cast<uint64_t>(pixel);
      hash *= mul;

      ++ptr_above;
      ++ptr_curr;
      ++ptr_below;
    }
  }

  return hash;
}
