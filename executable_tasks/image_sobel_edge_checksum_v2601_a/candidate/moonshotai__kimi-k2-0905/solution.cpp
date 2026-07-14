#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint8_t blend(const int p[9]) noexcept {
  const int gx = -p[0] + p[2] - 2 * p[3] + 2 * p[5] - p[6] + p[8];
  const int gy =  p[0] + 2 * p[1] + p[2] - p[6] - 2 * p[7] - p[8];
  const int mag = (gx < 0 ? -gx : gx) + (gy < 0 ? -gy : gy);
  return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

inline uint64_t update_fnv1a(uint64_t hash, uint8_t b) noexcept {
  hash ^= static_cast<uint64_t>(b);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sobel_edge_checksum(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    int iters)
{
  const int out_w = width - 2;
  const int out_h = height - 2;
  const std::size_t out_size = static_cast<std::size_t>(out_w) * static_cast<std::size_t>(out_h);
  std::vector<uint8_t> out;
  out.resize(out_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    uint8_t* dst = out.data();

    for (int y = 1; y < height - 1; ++y) {
      const uint8_t* row_t = image.data() + (y - 1) * width;
      const uint8_t* row_m = image.data() +  y      * width;
      const uint8_t* row_b = image.data() + (y + 1) * width;

      for (int x = 1; x < width - 1; ++x) {
        const uint8_t* tl = row_t + x - 1;
        const uint8_t* tm = row_t + x;
        const uint8_t* tr = row_t + x + 1;
        const uint8_t* ml = row_m + x - 1;
        const uint8_t* mm = row_m + x;
        const uint8_t* mr = row_m + x + 1;
        const uint8_t* bl = row_b + x - 1;
        const uint8_t* bm = row_b + x;
        const uint8_t* br = row_b + x + 1;

        int p[9] = {
          static_cast<int>(*tl), static_cast<int>(*tm), static_cast<int>(*tr),
          static_cast<int>(*ml), static_cast<int>(*mm), static_cast<int>(*mr),
          static_cast<int>(*bl), static_cast<int>(*bm), static_cast<int>(*br)
        };

        uint8_t val = blend(p);
        *dst++ = val;
        h = update_fnv1a(h, val);
      }
    }
    hash = h;
  }
  return hash;
}