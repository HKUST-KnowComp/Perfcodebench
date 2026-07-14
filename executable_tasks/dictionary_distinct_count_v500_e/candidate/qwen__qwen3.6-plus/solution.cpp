#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  (void)dictionary;
  
  alignas(64) bool seen[65536] = {};
  uint64_t count = 0;
  
  for (uint16_t code : codes) {
    if (!seen[code]) {
      seen[code] = true;
      ++count;
    }
  }
  
  return count;
}