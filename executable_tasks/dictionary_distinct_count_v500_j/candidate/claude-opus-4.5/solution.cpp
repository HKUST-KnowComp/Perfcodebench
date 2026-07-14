#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  
  const size_t dict_size = dictionary.size();
  const size_t codes_size = codes.size();
  const uint16_t* codes_ptr = codes.data();
  
  uint64_t count = 0;
  
  // Use a bitset to track which dictionary indices are used
  // Max uint16_t is 65535, so we need 65536 bits = 1024 uint64_t words
  constexpr size_t NUM_WORDS = 1024;
  uint64_t seen[NUM_WORDS];
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Mark all codes that appear
    size_t i = 0;
    
    // Process 4 codes at a time for better ILP
    const size_t unroll_limit = codes_size & ~size_t(3);
    for (; i < unroll_limit; i += 4) {
      uint16_t c0 = codes_ptr[i];
      uint16_t c1 = codes_ptr[i + 1];
      uint16_t c2 = codes_ptr[i + 2];
      uint16_t c3 = codes_ptr[i + 3];
      
      seen[c0 >> 6] |= uint64_t(1) << (c0 & 63);
      seen[c1 >> 6] |= uint64_t(1) << (c1 & 63);
      seen[c2 >> 6] |= uint64_t(1) << (c2 & 63);
      seen[c3 >> 6] |= uint64_t(1) << (c3 & 63);
    }
    
    // Handle remaining codes
    for (; i < codes_size; ++i) {
      uint16_t c = codes_ptr[i];
      seen[c >> 6] |= uint64_t(1) << (c & 63);
    }
    
    // Count set bits only in the range [0, dict_size)
    // We need to count bits for indices < dict_size
    const size_t full_words = dict_size >> 6;
    const size_t remaining_bits = dict_size & 63;
    
    count = 0;
    for (size_t w = 0; w < full_words; ++w) {
      count += __builtin_popcountll(seen[w]);
    }
    
    // Handle partial last word if dict_size is not a multiple of 64
    if (remaining_bits > 0) {
      uint64_t mask = (uint64_t(1) << remaining_bits) - 1;
      count += __builtin_popcountll(seen[full_words] & mask);
    }
  }
  
  return count;
}