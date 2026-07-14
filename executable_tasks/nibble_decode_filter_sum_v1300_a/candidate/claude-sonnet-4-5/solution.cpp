#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint8_t* data = packed.data();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 8 bytes at a time (16 nibbles)
    const std::size_t unroll_limit = size - (size % 8);
    for (; i < unroll_limit; i += 8) {
      uint32_t v0_lo = data[i] & 15u;
      uint32_t v0_hi = data[i] >> 4u;
      uint32_t v1_lo = data[i+1] & 15u;
      uint32_t v1_hi = data[i+1] >> 4u;
      uint32_t v2_lo = data[i+2] & 15u;
      uint32_t v2_hi = data[i+2] >> 4u;
      uint32_t v3_lo = data[i+3] & 15u;
      uint32_t v3_hi = data[i+3] >> 4u;
      uint32_t v4_lo = data[i+4] & 15u;
      uint32_t v4_hi = data[i+4] >> 4u;
      uint32_t v5_lo = data[i+5] & 15u;
      uint32_t v5_hi = data[i+5] >> 4u;
      uint32_t v6_lo = data[i+6] & 15u;
      uint32_t v6_hi = data[i+6] >> 4u;
      uint32_t v7_lo = data[i+7] & 15u;
      uint32_t v7_hi = data[i+7] >> 4u;
      
      local_sum += (v0_lo > thresh) ? v0_lo : 0;
      local_sum += (v0_hi > thresh) ? v0_hi : 0;
      local_sum += (v1_lo > thresh) ? v1_lo : 0;
      local_sum += (v1_hi > thresh) ? v1_hi : 0;
      local_sum += (v2_lo > thresh) ? v2_lo : 0;
      local_sum += (v2_hi > thresh) ? v2_hi : 0;
      local_sum += (v3_lo > thresh) ? v3_lo : 0;
      local_sum += (v3_hi > thresh) ? v3_hi : 0;
      local_sum += (v4_lo > thresh) ? v4_lo : 0;
      local_sum += (v4_hi > thresh) ? v4_hi : 0;
      local_sum += (v5_lo > thresh) ? v5_lo : 0;
      local_sum += (v5_hi > thresh) ? v5_hi : 0;
      local_sum += (v6_lo > thresh) ? v6_lo : 0;
      local_sum += (v6_hi > thresh) ? v6_hi : 0;
      local_sum += (v7_lo > thresh) ? v7_lo : 0;
      local_sum += (v7_hi > thresh) ? v7_hi : 0;
    }
    
    // Handle remaining bytes
    for (; i < size; ++i) {
      uint32_t lo = data[i] & 15u;
      uint32_t hi = data[i] >> 4u;
      local_sum += (lo > thresh) ? lo : 0;
      local_sum += (hi > thresh) ? hi : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}