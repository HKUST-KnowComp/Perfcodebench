#include "interface.h"

#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  const uint32_t* ids = row_ids.data();
  const size_t n = row_ids.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t r = 0;
    offsets[0] = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t row = ids[i];
      while (r < row && r < rows) {
        offsets[r + 1] = static_cast<uint32_t>(i);
        ++r;
      }
    }
    while (r < rows) {
      offsets[r + 1] = static_cast<uint32_t>(n);
      ++r;
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}