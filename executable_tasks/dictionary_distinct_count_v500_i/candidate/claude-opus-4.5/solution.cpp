#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Maximum possible dictionary index is 65535 (uint16_t max)
  // Use a bitset of 65536 bits = 1024 uint64_t words
  constexpr std::size_t kBitsetWords = 65536 / 64;
  uint64_t bitset[kBitsetWords];
  
  uint64_t count = 0;
  const uint16_t* codes_data = codes.data();
  const std::size_t codes_size = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitset, 0, sizeof(bitset));
    
    // Mark all codes that appear
    for (std::size_t i = 0; i < codes_size; ++i) {
      uint16_t code = codes_data[i];
      bitset[code >> 6] |= (1ULL << (code & 63));
    }
    
    // Count set bits using popcount
    count = 0;
    for (std::size_t i = 0; i < kBitsetWords; ++i) {
      count += static_cast<uint64_t>(__builtin_popcountll(bitset[i]));
    }
  }
  
  return count;
}