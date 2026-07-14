#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0u);
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // clear offsets to zero
    std::memset(offsets.data(), 0, (rows + 1) * sizeof(uint32_t));

    // count entries per row
    for (uint32_t r : row_ids) {
      ++offsets[r];
    }

    // inclusive prefix sum into offsets[1..rows]
    uint32_t sum = 0;
    for (size_t r = 0; r < rows; ++r) {
      uint32_t cnt = offsets[r];
      offsets[r] = sum;
      sum += cnt;
    }
    offsets[rows] = sum;

    // hash the offset array
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}