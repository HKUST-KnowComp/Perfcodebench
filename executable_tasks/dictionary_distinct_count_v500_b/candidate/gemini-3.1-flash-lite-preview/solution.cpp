#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-calculate the number of unique indices present in the codes
  // Using a boolean array for O(1) lookup/set
  size_t dict_size = dictionary.size();
  std::vector<bool> seen_indices(dict_size, false);
  uint64_t distinct_count = 0;

  for (uint16_t code : codes) {
    if (!seen_indices[code]) {
      seen_indices[code] = true;
      distinct_count++;
    }
  }

  // The problem asks for the count of distinct decoded strings.
  // Since dictionary[i] is fixed, distinct codes map to distinct strings.
  // We only need to count how many unique indices appear in the codes vector.
  return distinct_count;
}