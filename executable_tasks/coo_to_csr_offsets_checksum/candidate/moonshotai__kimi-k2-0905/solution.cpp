#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids,
                          uint32_t rows,
                          int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // zero initialize
    std::fill(offsets.begin(), offsets.end(), 0u);

    // histogram pass
    for (uint32_t r : row_ids) {
      ++offsets[r];
    }

    // prefix sum in-place
    uint32_t sum = 0;
    for (uint32_t r = 0; r <= rows; ++r) {
      uint32_t cnt = offsets[r];
      offsets[r] = sum;
      sum += cnt;
    }

    // hash
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}