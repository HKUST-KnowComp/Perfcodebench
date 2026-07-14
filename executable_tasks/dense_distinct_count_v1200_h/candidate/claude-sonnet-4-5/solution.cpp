#include "interface.h"
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  // Use a bitset represented as an array of uint64_t for fast bit operations
  const size_t num_words = (key_space + 63) / 64;
  uint64_t* bitset = new uint64_t[num_words];
  
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Clear bitset
    std::memset(bitset, 0, num_words * sizeof(uint64_t));
    
    // Set bits for each key
    for (uint32_t key : keys) {
      const size_t word_idx = key / 64;
      const size_t bit_idx = key % 64;
      bitset[word_idx] |= (1ULL << bit_idx);
    }
    
    // Count set bits
    count = 0;
    for (size_t i = 0; i < num_words; ++i) {
      count += __builtin_popcountll(bitset[i]);
    }
  }
  
  delete[] bitset;
  return count;
}