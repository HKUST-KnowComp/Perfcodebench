#include "interface.h"

#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  const size_t bitmap_size = (key_space + 63) / 64;
  std::vector<uint64_t> bitmap(bitmap_size);
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
    
    for (uint32_t key : keys) {
      const size_t word_idx = key >> 6;
      const uint64_t bit_mask = 1ULL << (key & 63);
      bitmap[word_idx] |= bit_mask;
    }
    
    count = 0;
    for (size_t i = 0; i < bitmap_size; ++i) {
      count += __builtin_popcountll(bitmap[i]);
    }
  }
  
  return count;
}