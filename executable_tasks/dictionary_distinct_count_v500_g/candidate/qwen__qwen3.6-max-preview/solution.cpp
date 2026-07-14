#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  uint64_t count = 0;
  uint8_t seen[65536];

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    count = 0;
    for (uint16_t c : codes) {
      if (!seen[c]) {
        seen[c] = 1;
        ++count;
      }
    }
  }
  return count;
}
