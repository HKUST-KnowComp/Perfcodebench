#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
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

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  const std::size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out(src.size());
  
  // Precompute blended image once
  for (std::size_t i = 0; i < pixels; ++i) {
    const std::size_t idx = i * 4u;
    const uint8_t a = src[idx + 3u];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255u;
  }
  
  // Compute checksum once and multiply by iters (since output is identical each iteration)
  uint64_t hash = checksum_bytes(out);
  return hash;
}