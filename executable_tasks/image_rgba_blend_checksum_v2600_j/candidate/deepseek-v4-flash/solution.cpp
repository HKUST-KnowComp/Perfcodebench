#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  const std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
  constexpr uint64_t fnv_offset = 1469598103934665603ULL;
  constexpr uint64_t fnv_prime = 1099511628211ULL;
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = fnv_offset;
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();

    for (std::size_t i = 0; i < pixels; ++i) {
      const uint8_t a = src_ptr[3];
      const uint32_t inv_a = 255u - a;

      // Blend red
      uint32_t r = (static_cast<uint32_t>(src_ptr[0]) * a +
                    static_cast<uint32_t>(dst_ptr[0]) * inv_a + 127u) / 255u;
      h ^= static_cast<uint64_t>(r);
      h *= fnv_prime;

      // Blend green
      uint32_t g = (static_cast<uint32_t>(src_ptr[1]) * a +
                    static_cast<uint32_t>(dst_ptr[1]) * inv_a + 127u) / 255u;
      h ^= static_cast<uint64_t>(g);
      h *= fnv_prime;

      // Blend blue
      uint32_t b = (static_cast<uint32_t>(src_ptr[2]) * a +
                    static_cast<uint32_t>(dst_ptr[2]) * inv_a + 127u) / 255u;
      h ^= static_cast<uint64_t>(b);
      h *= fnv_prime;

      // Alpha byte is always 255
      h ^= 255ULL;
      h *= fnv_prime;

      src_ptr += 4;
      dst_ptr += 4;
    }

    final_hash = h;
  }

  return final_hash;
}