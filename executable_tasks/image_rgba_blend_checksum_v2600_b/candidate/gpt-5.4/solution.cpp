#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * static_cast<uint32_t>(a) +
                               static_cast<uint32_t>(d) * static_cast<uint32_t>(255u - a) +
                               127u) /
                              255u);
}

static inline uint64_t checksum_buffer(const uint8_t* data, std::size_t n) {
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
  uint8_t* outp = out.data();
  const uint8_t* sp = src.data();
  const uint8_t* dp = dst.data();

  for (std::size_t i = 0; i < n; i += 4) {
    const uint8_t a = sp[i + 3];
    outp[i] = blend_byte(sp[i], dp[i], a);
    outp[i + 1] = blend_byte(sp[i + 1], dp[i + 1], a);
    outp[i + 2] = blend_byte(sp[i + 2], dp[i + 2], a);
    outp[i + 3] = 255u;
  }

  uint64_t hash = checksum_buffer(outp, n);
  for (int iter = 1; iter < iters; ++iter) {
    hash = checksum_buffer(outp, n);
  }
  return hash;
}
