#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  const std::size_t size = encoded.size();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process in chunks for better ILP
    while (i + 16 <= size) {
      // Unroll 4 varint decodes
      for (int unroll = 0; unroll < 4 && i < size; ++unroll) {
        uint32_t value = 0;
        uint32_t shift = 0;
        uint8_t byte;
        
        // Fast path: single-byte varint (most common)
        byte = data[i++];
        value = byte & 127u;
        if ((byte & 128u) == 0u) {
          local_sum += value;
          continue;
        }
        
        // Multi-byte varint
        shift = 7u;
        do {
          byte = data[i++];
          value |= static_cast<uint32_t>(byte & 127u) << shift;
          shift += 7u;
        } while ((byte & 128u) != 0u);
        
        local_sum += value;
      }
    }
    
    // Handle remaining bytes
    while (i < size) {
      uint32_t value = 0;
      uint32_t shift = 0;
      uint8_t byte;
      
      do {
        byte = data[i++];
        value |= static_cast<uint32_t>(byte & 127u) << shift;
        shift += 7u;
      } while ((byte & 128u) != 0u);
      
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}