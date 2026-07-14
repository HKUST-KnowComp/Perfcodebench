#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * static_cast<uint32_t>(a) +
                               static_cast<uint32_t>(d) * static_cast<uint32_t>(255u - a) +
                               127u) / 255u);
}

static inline uint64_t checksum_ptr(const uint8_t* data, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;

  const std::size_t n = src.size();
  if (n == 0 || iters <= 0) return 0;

  std::vector<uint8_t> out(n);
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  uint8_t* o = out.data();

  for (std::size_t i = 0; i < n; i += 4) {
    const uint8_t a = s[i + 3];
    o[i] = blend_byte(s[i], d[i], a);
    o[i + 1] = blend_byte(s[i + 1], d[i + 1], a);
    o[i + 2] = blend_byte(s[i + 2], d[i + 2], a);
    o[i + 3] = 255u;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_ptr(o, n);
  }
  return hash;
}
