#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;
  
  // We only need to identify which unique dictionary indices are present.
  // Since codes are uint16_t, there are at most 65536 unique entries.
  std::vector<bool> seen_indices(dictionary.size(), false);
  uint64_t distinct_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen_indices.begin(), seen_indices.end(), false);
    uint64_t current_count = 0;
    
    for (uint16_t code : codes) {
      if (!seen_indices[code]) {
        seen_indices[code] = true;
        current_count++;
      }
    }
    distinct_count = current_count;
  }
  
  return distinct_count;
}