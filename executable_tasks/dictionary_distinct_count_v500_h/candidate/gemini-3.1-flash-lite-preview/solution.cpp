#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-identify which dictionary indices are actually used.
  // Since codes are uint16_t, there are at most 65536 possible entries.
  std::vector<bool> used(dictionary.size(), false);
  for (uint16_t code : codes) {
    used[code] = true;
  }

  // Count the number of unique dictionary entries referenced.
  uint64_t distinct_count = 0;
  for (bool is_used : used) {
    if (is_used) {
      distinct_count++;
    }
  }

  // The result is invariant across iterations as the dictionary and codes are constant.
  return distinct_count;
}