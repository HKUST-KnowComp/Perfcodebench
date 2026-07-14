#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;
  
  // Use a bitset-like approach for small code spaces
  // uint16_t max is 65536, but we can use a simple boolean array
  // that's stack-allocated for typical dictionary sizes
  constexpr size_t MAX_DICT_SIZE = 65536;
  std::vector<bool> seen(std::min(dictionary.size(), MAX_DICT_SIZE), false);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset seen array
    std::fill(seen.begin(), seen.end(), false);
    
    // Mark which dictionary entries are referenced
    for (const uint16_t code : codes) {
      seen[code] = true;
    }
    
    // Count distinct entries
    count = 0;
    for (bool is_seen : seen) {
      count += is_seen;
    }
  }
  
  return count;
}