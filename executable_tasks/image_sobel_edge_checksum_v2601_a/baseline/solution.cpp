#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

uint8_t blend(const int* p) {
  const int gx = -p[0] + p[2] - 2 * p[3] + 2 * p[5] - p[6] + p[8];
  const int gy = p[0] + 2 * p[1] + p[2] - p[6] - 2 * p[7] - p[8];
  const int mag = std::abs(gx) + std::abs(gy);
  return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

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
  std::vector<uint8_t> out;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (int y = 1; y < height - 1; ++y) {
      for (int x = 1; x < width - 1; ++x) {
        int p[9];
        p[0] = image[static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x - 1)];
        p[1] = image[static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
        p[2] = image[static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x + 1)];
        p[3] = image[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x - 1)];
        p[4] = image[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
        p[5] = image[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x + 1)];
        p[6] = image[static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x - 1)];
        p[7] = image[static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)];
        p[8] = image[static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x + 1)];
        out.push_back(blend(p));
      }
    }
    hash = checksum_bytes(out);
  }
  return hash;
}
