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
  // Preallocate all reusable memory once outside iteration loop
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts efficiently with optimized fill
    std::fill(counts.begin(), counts.end(), 0);

    // Count row occurrences in linear, cache-friendly pass
    for (uint32_t row : row_ids) {
      counts[row]++;
    }

    // Compute CSR offset prefix sum
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[static_cast<size_t>(r + 1)] = offsets[static_cast<size_t>(r)] + counts[r];
    }

    // Compute identical checksum as original implementation
    final_hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      final_hash = mix(final_hash, v);
    }
  }
  return final_hash;
}