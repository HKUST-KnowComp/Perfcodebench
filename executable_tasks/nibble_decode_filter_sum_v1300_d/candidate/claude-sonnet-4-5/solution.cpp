#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint8_t* data = packed.data();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Process 4 bytes at a time for better ILP
    std::size_t i = 0;
    const std::size_t limit = size & ~3ULL;
    
    for (; i < limit; i += 4) {
      uint8_t b0 = data[i];
      uint8_t b1 = data[i + 1];
      uint8_t b2 = data[i + 2];
      uint8_t b3 = data[i + 3];
      
      uint32_t v0_lo = b0 & 15u;
      uint32_t v0_hi = b0 >> 4u;
      uint32_t v1_lo = b1 & 15u;
      uint32_t v1_hi = b1 >> 4u;
      uint32_t v2_lo = b2 & 15u;
      uint32_t v2_hi = b2 >> 4u;
      uint32_t v3_lo = b3 & 15u;
      uint32_t v3_hi = b3 >> 4u;
      
      // Branchless conditional accumulation
      local_sum += (v0_lo > thresh) ? v0_lo : 0;
      local_sum += (v0_hi > thresh) ? v0_hi : 0;
      local_sum += (v1_lo > thresh) ? v1_lo : 0;
      local_sum += (v1_hi > thresh) ? v1_hi : 0;
      local_sum += (v2_lo > thresh) ? v2_lo : 0;
      local_sum += (v2_hi > thresh) ? v2_hi : 0;
      local_sum += (v3_lo > thresh) ? v3_lo : 0;
      local_sum += (v3_hi > thresh) ? v3_hi : 0;
    }
    
    // Handle remaining bytes
    for (; i < size; ++i) {
      uint8_t b = data[i];
      uint32_t v_lo = b & 15u;
      uint32_t v_hi = b >> 4u;
      local_sum += (v_lo > thresh) ? v_lo : 0;
      local_sum += (v_hi > thresh) ? v_hi : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}