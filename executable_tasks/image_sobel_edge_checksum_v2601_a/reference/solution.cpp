#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  std::vector<uint8_t> out(static_cast<std::size_t>(width - 2) * static_cast<std::size_t>(height - 2));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* prev = image.data() + static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width);
      const uint8_t* cur = image.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(width);
      const uint8_t* next = image.data() + static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(width);
      uint8_t* dst = out.data() + static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width - 2);
      for (int x = 1; x < width - 1; ++x) {
        const int gx =
            -static_cast<int>(prev[x - 1]) + static_cast<int>(prev[x + 1]) -
            2 * static_cast<int>(cur[x - 1]) + 2 * static_cast<int>(cur[x + 1]) -
            static_cast<int>(next[x - 1]) + static_cast<int>(next[x + 1]);
        const int gy =
            static_cast<int>(prev[x - 1]) + 2 * static_cast<int>(prev[x]) + static_cast<int>(prev[x + 1]) -
            static_cast<int>(next[x - 1]) - 2 * static_cast<int>(next[x]) - static_cast<int>(next[x + 1]);
        const int mag = std::abs(gx) + std::abs(gy);
        dst[x - 1] = static_cast<uint8_t>(mag > 255 ? 255 : mag);
      }
    }
    hash = checksum_bytes(out);
  }
  return hash;
}
