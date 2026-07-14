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
  // Max 65536 possible codes (uint16_t), need 65536 bits = 8192 bytes = 1024 uint64_t
  alignas(64) uint64_t seen[1024];
  
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Mark all codes that appear
    for (size_t i = 0; i < num_codes; ++i) {
      uint16_t code = codes_ptr[i];
      seen[code >> 6] |= (1ULL << (code & 63));
    }
    
    // Count distinct codes (only up to dictionary size)
    count = 0;
    size_t num_words = (dict_size + 63) >> 6;
    for (size_t i = 0; i < num_words; ++i) {
      count += __builtin_popcountll(seen[i]);
    }
    
    // Handle partial last word if dictionary size not multiple of 64
    if (dict_size & 63) {
      // We may have counted bits beyond dict_size in the last word
      // But codes should only contain valid indices < dict_size, so this is fine
    }
  }
  
  return count;
}