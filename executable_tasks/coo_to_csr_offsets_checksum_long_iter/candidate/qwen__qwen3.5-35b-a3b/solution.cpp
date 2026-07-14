#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  uint64_t hash = 0;
  std::vector<uint32_t> offsets(rows + 1);
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0);
    size_t index = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      uint32_t count = 0;
      while (index < row_ids.size() && row_ids[index] == r) {
        ++count;
        ++index;
      }
      offsets[r + 1] = offsets[r] + count;
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}