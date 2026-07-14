#include "interface.h"

#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::vector<uint8_t> seen(dictionary.size(), 0);
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), 0);
    count = 0;
    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      if (!seen[idx]) {
        seen[idx] = 1;
        ++count;
      }
    }
  }
  return count;
}
