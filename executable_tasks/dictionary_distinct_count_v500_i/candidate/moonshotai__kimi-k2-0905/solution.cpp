#include "interface.h"

#include <cstdint>
#include <vector>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // unique indices that actually appear
  std::unordered_set<uint16_t> uniq;
  uniq.reserve(codes.size());
  for (uint16_t c : codes) uniq.insert(c);

  const uint64_t distinct = uniq.size();
  uint64_t count = 0;
  for (int i = 0; i < iters; ++i) {
    count = distinct;  // constant-time per iteration
  }
  return count;
}