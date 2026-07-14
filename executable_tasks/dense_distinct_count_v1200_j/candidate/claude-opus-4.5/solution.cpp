#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  // Use a flat bitmap for O(1) marking and counting
  // Each bit represents whether a key has been seen
  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words);
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear bitmap
    std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
    
    // Mark all keys in bitmap
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