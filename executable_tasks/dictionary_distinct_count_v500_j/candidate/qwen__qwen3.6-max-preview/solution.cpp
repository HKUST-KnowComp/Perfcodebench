#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  
  // Generation-tracked seen array avoids memset/clear overhead per iteration.
  // uint16_t codes range from 0 to 65535, so size 65536 covers all possibilities.
  std::vector<uint32_t> seen(65536, 0);
  uint32_t gen = 0;
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    ++gen;
    count = 0;
    for (uint16_t code : codes) {
      if (seen[code] != gen) {
        seen[code] = gen;
        ++count;
      }
    }
  }
  return count;
}
