#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint32_t a, uint32_t inv_a) {
  const uint32_t x = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * inv_a + 127u;
  return static_cast<uint8_t>((x * 0x8081U) >> 23);
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
  (void)width;
  (void)height;
  const std::size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out(src.size());
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* __restrict__ src_ptr = src.data();
    const uint8_t* __restrict__ dst_ptr = dst.data();
    uint8_t* __restrict__ out_ptr = out.data();

    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src_ptr[idx + 3u];
      const uint32_t a32 = static_cast<uint32_t>(a);
      const uint32_t inv_a = 255u - a32;

      out_ptr[idx] = blend_byte(src_ptr[idx], dst_ptr[idx], a32, inv_a);
      out_ptr[idx + 1u] = blend_byte(src_ptr[idx + 1u], dst_ptr[idx + 1u], a32, inv_a);
      out_ptr[idx + 2u] = blend_byte(src_ptr[idx + 2u], dst_ptr[idx + 2u], a32, inv_a);
      out_ptr[idx + 3u] = 255u;
    }
    hash = checksum_bytes(out);
  }
  return hash;
}