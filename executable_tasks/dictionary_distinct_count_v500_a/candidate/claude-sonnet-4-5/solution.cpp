#include "interface.h"

#include <cstdint>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<uint16_t> seen;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    seen.insert(codes.begin(), codes.end());
    count = static_cast<uint64_t>(seen.size());
  }
  return count;
}
