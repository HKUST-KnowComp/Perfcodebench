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
    
    // Count set bits (only up to dictionary size)
    size_t num_words = (dict_size + 63) >> 6;
    uint64_t c = 0;
    for (size_t i = 0; i < num_words; ++i) {
      c += __builtin_popcountll(bitset[i]);
    }
    count = c;
  }
  
  return count;
}
