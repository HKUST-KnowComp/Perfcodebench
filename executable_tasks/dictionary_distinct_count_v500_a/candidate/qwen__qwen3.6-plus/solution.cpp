#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  (void)iters;

  uint64_t count = 0;
  uint8_t seen[65536] = {};

  for (uint16_t code : codes) {
    if (!seen[code]) {
      seen[code] = 1;
      ++count;
    }
  }
  return count;
}
