#include "interface.h"

#include <cstring>
#include <immintrin.h>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  const size_t num_words = (key_space + 63) / 64;
  uint64_t* bitset = new uint64_t[num_words];
  
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitset, 0, num_words * sizeof(uint64_t));
    
    for (uint32_t key : keys) {
      bitset[key >> 6] |= (1ULL << (key & 63));
    }
    
    count = 0;
    for (size_t i = 0; i < num_words; ++i) {
      count += __builtin_popcountll(bitset[i]);
    }
  }
  
  delete[] bitset;
  return count;
}