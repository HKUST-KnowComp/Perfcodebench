#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<std::string> seen;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    seen.reserve(codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
      seen.insert(dictionary[static_cast<std::size_t>(codes[i])]);
    }
    count = static_cast<uint64_t>(seen.size());
  }
  return count;
}