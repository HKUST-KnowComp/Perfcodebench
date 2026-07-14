#include "interface.h"

#include <cstdint>
#include <vector>

#include "absl/container/flat_hash_set.h"

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  absl::flat_hash_set<uint16_t> seen;
  seen.reserve(codes.size());

  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    seen.clear();
    for (uint16_t code : codes) {
      seen.insert(code);
    }
    count = seen.size();
  }
  return count;
}