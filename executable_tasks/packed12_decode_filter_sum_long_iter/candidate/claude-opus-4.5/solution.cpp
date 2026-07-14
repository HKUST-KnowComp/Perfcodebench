#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* __restrict data = packed.data();
  const std::size_t size = packed.size();
  const std::size_t num_triplets = size / 3u;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 4 triplets (8 values) at a time for better ILP
    const std::size_t unroll_limit = (num_triplets / 4u) * 4u;
    
    for (; i < unroll_limit; i += 4u) {
      const std::size_t base = i * 3u;
      
      // Triplet 0
      uint32_t a0 = static_cast<uint32_t>(data[base]) | 
                    (static_cast<uint32_t>(data[base + 1u] & 0x0fu) << 8u);
      uint32_t b0 = (static_cast<uint32_t>(data[base + 1u]) >> 4u) | 
                    (static_cast<uint32_t>(data[base + 2u]) << 4u);
      
      // Triplet 1
      uint32_t a1 = static_cast<uint32_t>(data[base + 3u]) | 
                    (static_cast<uint32_t>(data[base + 4u] & 0x0fu) << 8u);
      uint32_t b1 = (static_cast<uint32_t>(data[base + 4u]) >> 4u) | 
                    (static_cast<uint32_t>(data[base + 5u]) << 4u);
      
      // Triplet 2
      uint32_t a2 = static_cast<uint32_t>(data[base + 6u]) | 
                    (static_cast<uint32_t>(data[base + 7u] & 0x0fu) << 8u);
      uint32_t b2 = (static_cast<uint32_t>(data[base + 7u]) >> 4u) | 
                    (static_cast<uint32_t>(data[base + 8u]) << 4u);
      
      // Triplet 3
      uint32_t a3 = static_cast<uint32_t>(data[base + 9u]) | 
                    (static_cast<uint32_t>(data[base + 10u] & 0x0fu) << 8u);
      uint32_t b3 = (static_cast<uint32_t>(data[base + 10u]) >> 4u) | 
                    (static_cast<uint32_t>(data[base + 11u]) << 4u);
      
      // Branchless accumulation
      local_sum += (a0 > thresh) ? a0 : 0u;
      local_sum += (b0 > thresh) ? b0 : 0u;
      local_sum += (a1 > thresh) ? a1 : 0u;
      local_sum += (b1 > thresh) ? b1 : 0u;
      local_sum += (a2 > thresh) ? a2 : 0u;
      local_sum += (b2 > thresh) ? b2 : 0u;
      local_sum += (a3 > thresh) ? a3 : 0u;
      local_sum += (b3 > thresh) ? b3 : 0u;
    }
    
    // Handle remaining triplets
    for (; i < num_triplets; ++i) {
      const std::size_t base = i * 3u;
      uint32_t a = static_cast<uint32_t>(data[base]) | 
                   (static_cast<uint32_t>(data[base + 1u] & 0x0fu) << 8u);
      uint32_t b = (static_cast<uint32_t>(data[base + 1u]) >> 4u) | 
                   (static_cast<uint32_t>(data[base + 2u]) << 4u);
      
      local_sum += (a > thresh) ? a : 0u;
      local_sum += (b > thresh) ? b : 0u;
    }
    
    sum = local_sum;
  }
  
  return sum;
}