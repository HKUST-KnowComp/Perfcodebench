#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

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
  const std::size_t total_pixels = src.size() / 4u;
  const std::size_t total_bytes = src.size();
  std::vector<uint8_t> out(total_bytes);
  
  // Initialize output alpha channel once (always 255, never modified)
  for (std::size_t i = 0; i < total_pixels; ++i) {
    out[i*4 + 3] = 255u;
  }
  
  const uint8_t* __restrict__ src_ptr = src.data();
  const uint8_t* __restrict__ dst_ptr = dst.data();
  uint8_t* __restrict__ out_ptr = out.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < total_pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint32_t a = src_ptr[idx + 3];
      const uint32_t inv_a = 255u - a;
      
      // Blend red channel
      out_ptr[idx] = static_cast<uint8_t>((
        static_cast<uint32_t>(src_ptr[idx]) * a +
        static_cast<uint32_t>(dst_ptr[idx]) * inv_a + 127u
      ) / 255u);
      // Blend green channel
      out_ptr[idx+1] = static_cast<uint8_t>((
        static_cast<uint32_t>(src_ptr[idx+1]) * a +
        static_cast<uint32_t>(dst_ptr[idx+1]) * inv_a + 127u
      ) / 255u);
      // Blend blue channel
      out_ptr[idx+2] = static_cast<uint8_t>((
        static_cast<uint32_t>(src_ptr[idx+2]) * a +
        static_cast<uint32_t>(dst_ptr[idx+2]) * inv_a + 127u
      ) / 255u);
    }
    hash = checksum_bytes(out);
  }
  return hash;
}