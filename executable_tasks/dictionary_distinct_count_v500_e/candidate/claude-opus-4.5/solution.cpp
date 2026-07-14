#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  
  uint64_t count = 0;
  
  // Use a bitset to track which dictionary indices are used
  // Max uint16_t is 65535, so we need 65536 bits = 1024 uint64_t
  constexpr size_t kBitsetSize = 1024;
  uint64_t seen[kBitsetSize];
  
  const uint16_t* codes_data = codes.data();
  const size_t codes_size = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Mark all used indices
    for (size_t i = 0; i < codes_size; ++i) {
      uint16_t code = codes_data[i];
      seen[code >> 6] |= (1ULL << (code & 63));
    }
    
    // Count set bits using popcount
    count = 0;
    for (size_t i = 0; i < kBitsetSize; ++i) {
      count += __builtin_popcountll(seen[i]);
    }
  }
  
  return count;
}