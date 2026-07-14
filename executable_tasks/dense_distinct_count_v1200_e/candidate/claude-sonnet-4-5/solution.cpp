#include "interface.h"

#include <cstring>
#include <memory>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  const size_t num_bytes = (key_space + 7) / 8;
  auto bitset = std::make_unique<uint8_t[]>(num_bytes);
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitset.get(), 0, num_bytes);
    
    for (uint32_t key : keys) {
      bitset[key >> 3] |= (1u << (key & 7));
    }
    
    count = 0;
    for (size_t i = 0; i < num_bytes; ++i) {
      count += __builtin_popcount(bitset[i]);
    }
  }
  
  return count;
}