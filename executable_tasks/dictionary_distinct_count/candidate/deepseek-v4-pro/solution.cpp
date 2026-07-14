#include "interface.h"

#include <string_view>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<std::string_view> seen;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (uint16_t code : codes) {
      seen.insert(dictionary[code]);
    }
    count = static_cast<uint64_t>(seen.size());
  }
  return count;
}