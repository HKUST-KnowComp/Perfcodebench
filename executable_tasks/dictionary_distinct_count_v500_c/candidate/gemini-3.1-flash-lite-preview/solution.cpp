#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-calculate which dictionary indices are actually used.
  // Since codes are uint16_t, there are at most 65536 possible indices.
  std::vector<bool> used(dictionary.size(), false);
  for (uint16_t code : codes) {
    used[code] = true;
  }

  uint64_t distinct_count = 0;
  for (bool is_used : used) {
    if (is_used) {
      distinct_count++;
    }
  }

  // The problem asks for the count of distinct decoded strings.
  // Since the mapping from code to string is 1:1 (dictionary index),
  // the number of distinct strings is simply the number of unique codes used.
  return distinct_count;
}