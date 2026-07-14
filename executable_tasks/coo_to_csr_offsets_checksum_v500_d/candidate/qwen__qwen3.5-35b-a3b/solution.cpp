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
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);

    if (!row_ids.empty()) {
      uint32_t current_row = row_ids[0];
      uint32_t count = 1;
      for (size_t i = 1; i < row_ids.size(); ++i) {
        if (row_ids[i] == current_row) {
          count++;
        } else {
          counts[current_row] = count;
          current_row = row_ids[i];
          count = 1;
        }
      }
      counts[current_row] = count;
    }

    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + counts[r];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}