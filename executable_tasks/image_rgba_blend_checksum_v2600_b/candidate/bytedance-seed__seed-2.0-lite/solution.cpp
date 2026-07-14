#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  const std::size_t total_pixels = src.size() / 4u;
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET;
    const uint8_t* __restrict__ src_ptr = src.data();
    const uint8_t* __restrict__ dst_ptr = dst.data();

    for (std::size_t i = 0; i < total_pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src_ptr[idx + 3u];
      const uint32_t a_u32 = static_cast<uint32_t>(a);
      const uint32_t one_minus_a_u32 = 255u - a_u32;

      // Blend red channel
      uint8_t br = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx]) * a_u32 +
                                         static_cast<uint32_t>(dst_ptr[idx]) * one_minus_a_u32 + 127u) / 255u);
      hash ^= static_cast<uint64_t>(br);
      hash *= FNV_PRIME;

      // Blend green channel
      uint8_t bg = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx+1u]) * a_u32 +
                                         static_cast<uint32_t>(dst_ptr[idx+1u]) * one_minus_a_u32 + 127u) / 255u);
      hash ^= static_cast<uint64_t>(bg);
      hash *= FNV_PRIME;

      // Blend blue channel
      uint8_t bb = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx+2u]) * a_u32 +
                                         static_cast<uint32_t>(dst_ptr[idx+2u]) * one_minus_a_u32 + 127u) / 255u);
      hash ^= static_cast<uint64_t>(bb);
      hash *= FNV_PRIME;

      // Output alpha is always 255 per original implementation
      hash ^= 255ULL;
      hash *= FNV_PRIME;
    }
    final_hash = hash;
  }
  return final_hash;
}