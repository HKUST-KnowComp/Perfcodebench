#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// The original formula is: (s * a + d * (255 - a) + 127) / 255
// This can be rewritten to minimize operations:
// Let term = s * a + d * (255 - a) + 127
// result = term / 255
// To avoid division, we can use: (term * 0x01010101) >> 24 if we were working with 24-bit, 
// but for exactness with the +127/255 rule, we use the property that 
// x / 255 ≈ (x * 257 + 1) / 65536 or similar. 
// However, for modern CPUs, a single division by a constant is often optimized to a multiply-shift.
// The most important optimization is avoiding push_back and using a pre-allocated buffer.

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
  
  // Pre-allocate output buffer to avoid reallocations
  std::vector<uint8_t> out(total_bytes);
  uint8_t* out_ptr = out.data();
  const uint8_t* src_ptr = src.data();
  const uint8_t* dst_ptr = dst.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src_ptr[idx + 3u];
      
      // Manual unrolling/inlining for the compiler to vectorize
      // We use the exact formula provided in the baseline to ensure correctness
      uint32_t a_inv = 255u - a;
      
      out_ptr[idx]     = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx]) * a + static_cast<uint32_t>(dst_ptr[idx]) * a_inv + 127u) / 255u);
      out_ptr[idx + 1] = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx + 1]) * a + static_cast<uint32_t>(dst_ptr[idx + 1]) * a_inv + 127u) / 255u);
      out_ptr[idx + 2] = static_cast<uint8_t>((static_cast<uint32_t>(src_ptr[idx + 2]) * a + static_cast<uint32_t>(dst_ptr[idx + 2]) * a_inv + 127u) / 255u);
      out_ptr[idx + 3] = 255u;
    }
    hash = checksum_bytes_fast(out_ptr, total_bytes);
  }
  return hash;
}