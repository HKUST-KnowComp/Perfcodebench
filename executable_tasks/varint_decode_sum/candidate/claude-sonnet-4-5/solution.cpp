#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  const std::size_t size = encoded.size();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    while (i < size) {
      uint32_t value = 0;
      uint32_t shift = 0;
      
      // Unrolled fast path for 1-5 byte varints (most common)
      uint8_t byte = data[i++];
      value = byte & 127u;
      if ((byte & 128u) == 0) {
        local_sum += value;
        continue;
      }
      
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 7;
      if ((byte & 128u) == 0) {
        local_sum += value;
        continue;
      }
      
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 14;
      if ((byte & 128u) == 0) {
        local_sum += value;
        continue;
      }
      
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 21;
      if ((byte & 128u) == 0) {
        local_sum += value;
        continue;
      }
      
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 28;
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}