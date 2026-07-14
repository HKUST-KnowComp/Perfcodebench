#include "interface.h"
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)iters;
  std::vector<uint8_t> seen(dictionary.size(), 0);
  uint64_t count = 0;
  for (uint16_t c : codes) {
    if (!seen[c]) {
      seen[c] = 1;
      ++count;
    }
  }
  return count;
}