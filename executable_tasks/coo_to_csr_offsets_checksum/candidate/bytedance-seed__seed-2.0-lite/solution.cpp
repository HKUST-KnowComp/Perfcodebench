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
  // Preallocate all working buffers once to avoid repeated allocation overhead
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset row counts to zero, optimized for contiguous memory
    std::fill(counts.begin(), counts.end(), 0);

    // Fast linear scan to count row occurrences, perfect cache locality
    for (uint32_t row : row_ids) {
      counts[row]++;
    }

    // Compute CSR offset prefix sum in a tight, cache-friendly loop
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + counts[r];
    }

    // Compute identical checksum to original baseline for correctness
    final_hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      final_hash = mix(final_hash, v);
    }
  }

  return final_hash;
}