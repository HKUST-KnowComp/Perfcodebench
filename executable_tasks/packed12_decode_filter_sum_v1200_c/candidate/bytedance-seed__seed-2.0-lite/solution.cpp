#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Hoist fixed input metadata and constants outside all loops to avoid recomputation
  const uint8_t* in_data = packed.data();
  const std::size_t in_size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t final_sum = 0;

  // Run full computation for all iterations per harness requirements
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    // Process every 3-byte block to decode two 12-bit values and sum in one pass
    for (std::size_t i = 0; i < in_size; i += 3) {
      // Exact same bitwise decoding logic as baseline to preserve 100% correctness
      const uint32_t a = static_cast<uint32_t>(in_data[i]) | (static_cast<uint32_t>(in_data[i+1] & 0x0Fu) << 8u);
      const uint32_t b = (static_cast<uint32_t>(in_data[i+1] >> 4u)) | (static_cast<uint32_t>(in_data[i+2]) << 4u);
      // Fused filter and add operations to avoid storing intermediate values
      if (a > thresh) current_sum += a;
      if (b > thresh) current_sum += b;
    }
    final_sum = current_sum;
  }
  return final_sum;
}