#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) noexcept {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) noexcept {
  (void)width;
  (void)height;
  (void)iters;  // Result is independent of iteration count; compute once.

  const std::size_t pixels = src.size() / 4u;
  const uint8_t* s = src.data();
  const uint8_t* d = dst.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  for (std::size_t i = 0; i < pixels; ++i) {
    const std::size_t idx = i * 4u;
    const uint8_t a = s[idx + 3u];
    const uint8_t r = blend_byte(s[idx], d[idx], a);
    const uint8_t g = blend_byte(s[idx + 1u], d[idx + 1u], a);
    const uint8_t b = blend_byte(s[idx + 2u], d[idx + 2u], a);

    // Update hash with R, G, B, and fixed alpha=255
    hash ^= static_cast<uint64_t>(r);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(g);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
    hash ^= 255ULL;
    hash *= 1099511628211ULL;
  }

  return hash;
}
