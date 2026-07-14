#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Since codes are uint16_t, max possible value is 65535
  // We use a bitset to track which codes appear
  // 65536 bits = 1024 uint64_t words = 8KB (fits in L1 cache)
  
  uint64_t count = 0;
  alignas(64) uint64_t seen[1024];
  
  const uint16_t* __restrict codes_ptr = codes.data();
  const size_t n = codes.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    
    // Set bits for each code that appears
    for (size_t i = 0; i < n; ++i) {
      uint16_t c = codes_ptr[i];
      seen[c >> 6] |= (1ULL << (c & 63));
    }
    
    // Count set bits (popcount)
    count = 0;
    for (int i = 0; i < 1024; ++i) {
      count += __builtin_popcountll(seen[i]);
    }
  }
  
  return count;
}