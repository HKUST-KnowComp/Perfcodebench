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
  // Max 65536 possible codes (uint16_t), need 65536 bits = 1024 uint64_t
  constexpr size_t BITSET_SIZE = 1024;
  uint64_t bitset[BITSET_SIZE];
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitset, 0, sizeof(bitset));
    
    // Mark all codes that appear
    for (size_t i = 0; i < num_codes; ++i) {
      uint16_t code = codes_ptr[i];
      bitset[code >> 6] |= (1ULL << (code & 63));
    }
    
    // Count set bits only up to dictionary size
    size_t words_needed = (dict_size + 63) >> 6;
    count = 0;
    for (size_t i = 0; i < words_needed; ++i) {
      count += __builtin_popcountll(bitset[i]);
    }
    
    // Handle partial last word if dictionary size not multiple of 64
    // Actually the above already counts all set bits in those words,
    // but codes should be < dict_size, so this is correct.
  }
  
  return count;
}
