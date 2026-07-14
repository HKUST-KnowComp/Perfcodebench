#include "interface.h"

#include <cstring>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  // Allocate bitmap: 1 bit per possible key
  size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words);
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear bitmap
    std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
    
    // Mark each key in bitmap
    for (uint32_t key : keys) {
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }
    
    // Count set bits using popcount
    count = 0;
    for (size_t i = 0; i < num_words; ++i) {
      count += __builtin_popcountll(bitmap[i]);
    }
  }
  
  return count;
}
