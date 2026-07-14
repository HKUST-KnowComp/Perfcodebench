#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;
  
  // Use a bitset-like approach for small code spaces
  // uint16_t max is 65536, but we can use a vector<bool> or direct array
  constexpr size_t MAX_CODES = 65536;
  std::vector<bool> seen(MAX_CODES, false);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset seen array
    std::fill(seen.begin(), seen.end(), false);
    
    // Mark which codes appear
    for (uint16_t code : codes) {
      seen[code] = true;
    }
    
    // Count distinct codes
    count = 0;
    for (bool s : seen) {
      count += s;
    }
  }
  
  return count;
}