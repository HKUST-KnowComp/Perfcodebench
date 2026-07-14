#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* __restrict data = packed.data();
  const std::size_t size = packed.size();
  const std::size_t num_triplets = size / 3u;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 4 triplets (12 bytes -> 8 values) at a time for better ILP
    const std::size_t unroll_limit = (num_triplets / 4u) * 4u;
    
    for (; i < unroll_limit; i += 4u) {
      const std::size_t base = i * 3u;
      
      // Triplet 0
      const uint32_t b0 = data[base];
      const uint32_t b1 = data[base + 1u];
      const uint32_t b2 = data[base + 2u];
      const uint32_t a0 = b0 | ((b1 & 0x0fu) << 8u);
      const uint32_t a1 = (b1 >> 4u) | (b2 << 4u);
      
      // Triplet 1
      const uint32_t b3 = data[base + 3u];
      const uint32_t b4 = data[base + 4u];
      const uint32_t b5 = data[base + 5u];
      const uint32_t a2 = b3 | ((b4 & 0x0fu) << 8u);
      const uint32_t a3 = (b4 >> 4u) | (b5 << 4u);
      
      // Triplet 2
      const uint32_t b6 = data[base + 6u];
      const uint32_t b7 = data[base + 7u];
      const uint32_t b8 = data[base + 8u];
      const uint32_t a4 = b6 | ((b7 & 0x0fu) << 8u);
      const uint32_t a5 = (b7 >> 4u) | (b8 << 4u);
      
      // Triplet 3
      const uint32_t b9 = data[base + 9u];
      const uint32_t b10 = data[base + 10u];
      const uint32_t b11 = data[base + 11u];
      const uint32_t a6 = b9 | ((b10 & 0x0fu) << 8u);
      const uint32_t a7 = (b10 >> 4u) | (b11 << 4u);
      
      // Branchless accumulation using arithmetic
      local_sum += (a0 > thresh) ? a0 : 0u;
      local_sum += (a1 > thresh) ? a1 : 0u;
      local_sum += (a2 > thresh) ? a2 : 0u;
      local_sum += (a3 > thresh) ? a3 : 0u;
      local_sum += (a4 > thresh) ? a4 : 0u;
      local_sum += (a5 > thresh) ? a5 : 0u;
      local_sum += (a6 > thresh) ? a6 : 0u;
      local_sum += (a7 > thresh) ? a7 : 0u;
    }
    
    // Handle remaining triplets
    for (; i < num_triplets; ++i) {
      const std::size_t base = i * 3u;
      const uint32_t b0 = data[base];
      const uint32_t b1 = data[base + 1u];
      const uint32_t b2 = data[base + 2u];
      const uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
      const uint32_t b = (b1 >> 4u) | (b2 << 4u);
      
      local_sum += (a > thresh) ? a : 0u;
      local_sum += (b > thresh) ? b : 0u;
    }
    
    sum = local_sum;
  }
  
  return sum;
}