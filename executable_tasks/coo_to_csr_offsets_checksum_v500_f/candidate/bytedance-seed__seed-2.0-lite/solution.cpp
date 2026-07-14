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
  // Preallocate offsets buffer once to avoid repeated heap allocations
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t last_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets to zero for fresh computation each iteration
    std::fill(offsets.begin(), offsets.end(), 0);

    // Count row occurrences directly in offsets array (leveraging sorted COO)
    for (uint32_t row : row_ids) {
      offsets[row + 1]++;
    }

    // Compute prefix sum to build valid CSR offsets
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] += offsets[r];
    }

    // Calculate hash identically to baseline to preserve correctness
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    last_hash = hash;
  }

  return last_hash;
}