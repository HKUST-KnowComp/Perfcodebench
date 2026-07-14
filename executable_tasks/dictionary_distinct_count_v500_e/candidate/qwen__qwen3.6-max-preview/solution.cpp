#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <string>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)dictionary;
  uint64_t count = 0;
  std::vector<uint32_t> seen_tag(65536, 0);
  uint32_t current_tag = 1;
  
  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint16_t c : codes) {
      if (seen_tag[c] != current_tag) {
        seen_tag[c] = current_tag;
        ++count;
      }
    }
    ++current_tag;
  }
  return count;
}