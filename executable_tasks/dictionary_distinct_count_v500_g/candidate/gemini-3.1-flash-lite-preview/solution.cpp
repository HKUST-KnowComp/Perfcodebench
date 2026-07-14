#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;
  
  size_t dict_size = dictionary.size();
  std::vector<bool> seen(dict_size, false);
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), false);
    uint64_t current_count = 0;
    for (uint16_t code : codes) {
      if (!seen[code]) {
        seen[code] = true;
        current_count++;
      }
    }
    count = current_count;
  }
  return count;
}