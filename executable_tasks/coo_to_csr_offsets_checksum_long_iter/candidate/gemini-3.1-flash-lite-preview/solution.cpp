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

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);
  std::vector<uint32_t> counts(static_cast<std::size_t>(rows), 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    std::fill(counts.begin(), counts.end(), 0);
    
    // Count occurrences
    for (uint32_t row : row_ids) {
      if (row < rows) {
        counts[row]++;
      }
    }

    // Build CSR offsets
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + counts[r];
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}