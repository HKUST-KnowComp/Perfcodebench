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
      uint32_t v0 = data[i] & 15u;
      uint32_t v1 = data[i] >> 4u;
      uint32_t v2 = data[i+1] & 15u;
      uint32_t v3 = data[i+1] >> 4u;
      uint32_t v4 = data[i+2] & 15u;
      uint32_t v5 = data[i+2] >> 4u;
      uint32_t v6 = data[i+3] & 15u;
      uint32_t v7 = data[i+3] >> 4u;
      uint32_t v8 = data[i+4] & 15u;
      uint32_t v9 = data[i+4] >> 4u;
      uint32_t v10 = data[i+5] & 15u;
      uint32_t v11 = data[i+5] >> 4u;
      uint32_t v12 = data[i+6] & 15u;
      uint32_t v13 = data[i+6] >> 4u;
      uint32_t v14 = data[i+7] & 15u;
      uint32_t v15 = data[i+7] >> 4u;
      
      // Branchless conditional accumulation
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
      local_sum += (v2 > thresh) ? v2 : 0;
      local_sum += (v3 > thresh) ? v3 : 0;
      local_sum += (v4 > thresh) ? v4 : 0;
      local_sum += (v5 > thresh) ? v5 : 0;
      local_sum += (v6 > thresh) ? v6 : 0;
      local_sum += (v7 > thresh) ? v7 : 0;
      local_sum += (v8 > thresh) ? v8 : 0;
      local_sum += (v9 > thresh) ? v9 : 0;
      local_sum += (v10 > thresh) ? v10 : 0;
      local_sum += (v11 > thresh) ? v11 : 0;
      local_sum += (v12 > thresh) ? v12 : 0;
      local_sum += (v13 > thresh) ? v13 : 0;
      local_sum += (v14 > thresh) ? v14 : 0;
      local_sum += (v15 > thresh) ? v15 : 0;
    }
    
    // Handle remaining bytes
    for (; i < size; ++i) {
      uint32_t v0 = data[i] & 15u;
      uint32_t v1 = data[i] >> 4u;
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}