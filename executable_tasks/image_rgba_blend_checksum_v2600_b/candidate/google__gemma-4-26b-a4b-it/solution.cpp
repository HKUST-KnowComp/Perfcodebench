#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// The original formula is: (s * a + d * (255 - a) + 127) / 255
// To maintain exact correctness, we must use the same integer division.
// We can optimize the division by 255 using: (x + (x >> 8) + 1) >> 8 is not quite right for all cases,
// but (x + 127) / 255 is the target. 
// Actually, the most robust way to optimize (val / 255) is (val + 1 + (val >> 8)) >> 8 if we want to avoid division.
// However, to ensure 100% bit-exactness with the baseline's (val + 127) / 255, 
// we use the property that (x / 255) == ((x + 1 + (x >> 8)) >> 8) is an approximation.
// Let's use the exact integer math to avoid any rounding errors.

inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
    uint32_t val = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a) + 127u;
    return static_cast<uint8_t>(val / 255u);
}

inline uint64_t checksum_bytes_fast(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  const std::size_t total_bytes = src.size();
  const std::size_t pixels = total_bytes / 4u;
  
  // Pre-allocate output buffer to avoid reallocations and push_back overhead
  std::vector<uint8_t> out(total_bytes);
  uint8_t* out_ptr = out.data();
  const uint8_t* src_ptr = src.data();
  const uint8_t* dst_ptr = dst.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src_ptr[idx + 3u];
      
      // Manual unrolling of the blend_byte calls for the 3 channels
      // We use the exact formula to ensure correctness.
      uint32_t a_inv = 255u - a;
      
      uint32_t r_val = static_cast<uint32_t>(src_ptr[idx]) * a + static_cast<uint32_t>(dst_ptr[idx]) * a_inv + 127u;
      out_ptr[idx] = static_cast<uint8_t>(r_val / 255u);
      
      uint32_t g_val = static_cast<uint32_t>(src_ptr[idx + 1u]) * a + static_cast<uint32_t>(dst_ptr[idx + 1u]) * a_inv + 127u;
      out_ptr[idx + 1] = static_cast<uint8_t>(g_val / 255u);
      
      uint32_t b_val = static_cast<uint32_t>(src_ptr[idx + 2u]) * a + static_cast<uint32_t>(dst_ptr[idx + 2u]) * a_inv + 127u;
      out_ptr[idx + 2] = static_cast<uint8_t>(b_val / 255u);
      
      out_ptr[idx + 3] = 255u;
    }
    hash = checksum_bytes_fast(out_ptr, total_bytes);
  }
  return hash;
}
