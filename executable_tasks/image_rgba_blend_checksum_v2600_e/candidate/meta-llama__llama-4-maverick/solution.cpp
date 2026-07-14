#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
}

uint64_t checksum_bytes(const uint8_t* bytes, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
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
  const std::size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out(src.size());
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src[idx + 3u];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out.data(), out.size());
  }
  return hash;
}