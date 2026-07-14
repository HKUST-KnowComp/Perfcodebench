#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  const size_t dict_size = dictionary.size();
  const size_t num_codes = codes.size();
  const uint16_t* codes_ptr = codes.data();
  
  // Bitset to track which dictionary indices are used
  // Max dictionary size is 65536 (uint16_t range)
  // 65536 bits = 8192 bytes = 1024 uint64_t
  constexpr size_t BITSET_SIZE = 1024;
  uint64_t seen[BITSET_SIZE];
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Mark all codes as seen in bitset
    for (size_t i = 0; i < num_codes; ++i) {
      uint16_t code = codes_ptr[i];
      seen[code >> 6] |= (1ULL << (code & 63));
    }
    
    // Count set bits only up to dictionary size
    count = 0;
    size_t full_words = dict_size >> 6;
    size_t remaining_bits = dict_size & 63;
    
    for (size_t i = 0; i < full_words; ++i) {
      count += __builtin_popcountll(seen[i]);
    }
    
    if (remaining_bits > 0) {
      uint64_t mask = (1ULL << remaining_bits) - 1;
      count += __builtin_popcountll(seen[full_words] & mask);
    }
  }
  
  return count;
}