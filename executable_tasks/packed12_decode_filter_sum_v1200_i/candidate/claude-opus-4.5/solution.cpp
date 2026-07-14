#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* __restrict data = packed.data();
  const std::size_t size = packed.size();
  const std::size_t triplets = size / 3u;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc = 0;
    std::size_t i = 0;
    
    // Process 12 bytes at a time (4 triplets = 8 values)
    const std::size_t fast_end = (triplets / 4u) * 4u;
    
    for (; i < fast_end; i += 4u) {
      const std::size_t base = i * 3u;
      
      // Triplet 0
      uint32_t b0 = data[base];
      uint32_t b1 = data[base + 1u];
      uint32_t b2 = data[base + 2u];
      uint32_t a0 = b0 | ((b1 & 0x0fu) << 8u);
      uint32_t a1 = (b1 >> 4u) | (b2 << 4u);
      
      // Triplet 1
      uint32_t b3 = data[base + 3u];
      uint32_t b4 = data[base + 4u];
      uint32_t b5 = data[base + 5u];
      uint32_t a2 = b3 | ((b4 & 0x0fu) << 8u);
      uint32_t a3 = (b4 >> 4u) | (b5 << 4u);
      
      // Triplet 2
      uint32_t b6 = data[base + 6u];
      uint32_t b7 = data[base + 7u];
      uint32_t b8 = data[base + 8u];
      uint32_t a4 = b6 | ((b7 & 0x0fu) << 8u);
      uint32_t a5 = (b7 >> 4u) | (b8 << 4u);
      
      // Triplet 3
      uint32_t b9 = data[base + 9u];
      uint32_t b10 = data[base + 10u];
      uint32_t b11 = data[base + 11u];
      uint32_t a6 = b9 | ((b10 & 0x0fu) << 8u);
      uint32_t a7 = (b10 >> 4u) | (b11 << 4u);
      
      // Branchless accumulation using arithmetic mask
      acc += a0 * static_cast<uint64_t>(a0 > thresh);
      acc += a1 * static_cast<uint64_t>(a1 > thresh);
      acc += a2 * static_cast<uint64_t>(a2 > thresh);
      acc += a3 * static_cast<uint64_t>(a3 > thresh);
      acc += a4 * static_cast<uint64_t>(a4 > thresh);
      acc += a5 * static_cast<uint64_t>(a5 > thresh);
      acc += a6 * static_cast<uint64_t>(a6 > thresh);
      acc += a7 * static_cast<uint64_t>(a7 > thresh);
    }
    
    // Handle remaining triplets
    for (; i < triplets; ++i) {
      const std::size_t base = i * 3u;
      uint32_t b0 = data[base];
      uint32_t b1 = data[base + 1u];
      uint32_t b2 = data[base + 2u];
      uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
      uint32_t b = (b1 >> 4u) | (b2 << 4u);
      acc += a * static_cast<uint64_t>(a > thresh);
      acc += b * static_cast<uint64_t>(b > thresh);
    }
    
    sum = acc;
  }
  
  return sum;
}