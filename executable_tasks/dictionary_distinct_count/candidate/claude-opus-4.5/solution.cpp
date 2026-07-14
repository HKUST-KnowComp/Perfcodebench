#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Maximum possible dictionary indices for uint16_t codes
  constexpr std::size_t kMaxCodes = 65536;
  constexpr std::size_t kNumWords = kMaxCodes / 64;
  
  // Use stack-allocated bitset for speed
  uint64_t seen[kNumWords];
  
  uint64_t count = 0;
  const std::size_t n = codes.size();
  const uint16_t* __restrict code_ptr = codes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Mark all codes that appear
    for (std::size_t i = 0; i < n; ++i) {
      uint16_t c = code_ptr[i];
      seen[c >> 6] |= (uint64_t{1} << (c & 63));
    }
    
    // Count set bits using popcount
    uint64_t total = 0;
    for (std::size_t i = 0; i < kNumWords; ++i) {
      total += __builtin_popcountll(seen[i]);
    }
    count = total;
  }
  
  return count;
}