#include "interface.h"
#include <algorithm>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (keys.empty()) return 0;
  
  // Find the actual range of keys to minimize memory usage
  uint32_t min_key = keys[0];
  uint32_t max_key = keys[0];
  for (uint32_t key : keys) {
    if (key < min_key) min_key = key;
    if (key > max_key) max_key = key;
  }
  
  uint32_t range = max_key - min_key + 1;
  
  // Use bitset approach for better cache locality and performance
  // Each bit represents whether a key has been seen
  const size_t num_words = (range + 63) / 64;
  
  uint64_t count = 0;
  
  // For small ranges, use stack allocation
  if (num_words <= 1024) {
    uint64_t bitset[1024];
    
    for (int iter = 0; iter < iters; ++iter) {
      std::memset(bitset, 0, num_words * sizeof(uint64_t));
      
      for (uint32_t key : keys) {
        uint32_t offset = key - min_key;
        uint32_t word_idx = offset / 64;
        uint32_t bit_idx = offset % 64;
        bitset[word_idx] |= (1ULL << bit_idx);
      }
      
      // Count set bits
      count = 0;
      for (size_t i = 0; i < num_words; ++i) {
        count += __builtin_popcountll(bitset[i]);
      }
    }
  } else {
    // For larger ranges, use heap allocation
    std::vector<uint64_t> bitset(num_words);
    
    for (int iter = 0; iter < iters; ++iter) {
      std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));
      
      for (uint32_t key : keys) {
        uint32_t offset = key - min_key;
        uint32_t word_idx = offset / 64;
        uint32_t bit_idx = offset % 64;
        bitset[word_idx] |= (1ULL << bit_idx);
      }
      
      // Count set bits
      count = 0;
      for (size_t i = 0; i < num_words; ++i) {
        count += __builtin_popcountll(bitset[i]);
      }
    }
  }
  
  return count;
}