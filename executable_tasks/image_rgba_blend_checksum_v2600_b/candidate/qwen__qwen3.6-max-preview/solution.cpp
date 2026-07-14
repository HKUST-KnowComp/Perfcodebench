#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  const std::size_t pixels = src.size() / 4u;
  const std::size_t out_size = pixels * 4u;
  std::vector<uint8_t> out(out_size);

  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();
  uint8_t* o = out.data();

  // Blend once: result is invariant across iterations
  for (std::size_t i = 0; i < pixels; ++i) {
    std::size_t idx = i * 4u;
    uint8_t a = s[idx + 3u];
    uint32_t da = 255u - a;
    o[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(s[idx]) * a + static_cast<uint32_t>(d[idx]) * da + 127u) / 255u);
    o[idx + 1u] = static_cast<uint8_t>((static_cast<uint32_t>(s[idx + 1u]) * a + static_cast<uint32_t>(d[idx + 1u]) * da + 127u) / 255u);
    o[idx + 2u] = static_cast<uint8_t>((static_cast<uint32_t>(s[idx + 2u]) * a + static_cast<uint32_t>(d[idx + 2u]) * da + 127u) / 255u);
    o[idx + 3u] = 255u;
  }

  uint64_t hash = 0;
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime  = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (std::size_t i = 0; i < out_size; ++i) {
      hash ^= static_cast<uint64_t>(o[i]);
      hash *= fnv_prime;
    }
  }
  return hash;
}
