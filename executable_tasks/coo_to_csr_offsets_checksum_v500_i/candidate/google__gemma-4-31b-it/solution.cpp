#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Pre-allocate memory to avoid repeated allocations in the loop
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    std::fill(counts.begin(), counts.end(), 0);

    // Since row_ids are sorted, we can count occurrences efficiently
    // Even if they weren't sorted, a direct array is faster than a map
    for (uint32_t row : row_ids) {
      counts[row]++;
    }

    // Compute CSR offsets (prefix sum of counts)
    uint32_t current_offset = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      current_offset += counts[r];
      offsets[r + 1] = current_offset;
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}