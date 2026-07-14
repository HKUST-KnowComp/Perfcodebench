#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;
  if (iters <= 0) return 0;

  uint64_t last_count = 0;
  size_t dict_size = dictionary.size();
  
  // Pre-allocate a presence vector to avoid re-allocation.
  // Using vector<bool> or a manual bitset is efficient for marking seen indices.
  std::vector<bool> seen(dict_size, false);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the seen vector efficiently
    std::fill(seen.begin(), seen.end(), false);
    
    uint64_t distinct_in_iter = 0;
    for (uint16_t code : codes) {
      // The problem implies codes are valid indices into the dictionary.
      // We mark the index as seen. If it was not seen before, increment count.
      if (!seen[code]) {
        seen[code] = true;
        distinct_in_iter++;
      }
    }
    last_count = distinct_in_iter;
  }

  return last_count;
}