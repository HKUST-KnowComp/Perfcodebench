#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t sobel_pixel(
    const uint8_t* prev,
    const uint8_t* curr,
    const uint8_t* next,
    int x) {
  const int tl = prev[x - 1];
  const int tc = prev[x];
  const int tr = prev[x + 1];
  const int ml = curr[x - 1];
  const int mr = curr[x + 1];
  const int bl = next[x - 1];
  const int bc = next[x];
  const int br = next[x + 1];

  const int gx = -tl + tr - (ml << 1) + (mr << 1) - bl + br;
  const int gy = tl + (tc << 1) + tr - bl - (bc << 1) - br;

  const int ax = gx < 0 ? -gx : gx;
  const int ay = gy < 0 ? -gy : gy;
  const int mag = ax + ay;
  return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

}  // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters) {
  const int out_w = width - 2;
  const int out_h = height - 2;

  uint64_t hash = 0;
  if (iters <= 0) return hash;
  if (out_w <= 0 || out_h <= 0) {
    constexpr uint64_t kOffset = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) hash = kOffset;
    return hash;
  }

  const uint8_t* img = image.data();
  const int stride = width;
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kOffset;

    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* prev = img + static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(stride);
      const uint8_t* curr = img + static_cast<std::size_t>(y) * static_cast<std::size_t>(stride);
      const uint8_t* next = img + static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(stride);

      for (int x = 1; x < width - 1; ++x) {
        const uint8_t v = sobel_pixel(prev, curr, next, x);
        h ^= static_cast<uint64_t>(v);
        h *= kPrime;
      }
    }

    hash = h;
  }

  return hash;
}
