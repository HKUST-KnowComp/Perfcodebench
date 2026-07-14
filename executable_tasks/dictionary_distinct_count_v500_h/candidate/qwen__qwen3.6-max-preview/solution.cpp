#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <cstring>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  
  uint8_t seen[65536];
  std::vector<const std::string*> ptrs;
  ptrs.reserve(std::min(dictionary.size(), size_t(65536)));
  uint64_t count = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(seen, 0, sizeof(seen));
    for (uint16_t c : codes) {
      seen[c] = 1;
    }
    
    ptrs.clear();
    size_t limit = dictionary.size();
    if (limit > 65536) limit = 65536;
    for (size_t i = 0; i < limit; ++i) {
      if (seen[i]) {
        ptrs.push_back(&dictionary[i]);
      }
    }
    
    std::sort(ptrs.begin(), ptrs.end(), [](const std::string* a, const std::string* b) {
      return *a < *b;
    });
    count = static_cast<uint64_t>(std::unique(ptrs.begin(), ptrs.end(), [](const std::string* a, const std::string* b) {
      return *a == *b;
    }) - ptrs.begin());
  }
  return count;
}