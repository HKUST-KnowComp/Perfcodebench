#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  (void)iters;
  bool seen[65536] = {};
  uint64_t count = 0;
  for (uint16_t c : codes) {
    if (!seen[c]) {
      seen[c] = true;
      ++count;
    }
  }
  return count;
}