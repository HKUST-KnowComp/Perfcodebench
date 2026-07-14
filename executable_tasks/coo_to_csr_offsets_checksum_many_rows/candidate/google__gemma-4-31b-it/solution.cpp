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
  // Pre-allocate buffers to avoid repeated allocations in the loop
  std::vector<uint32_t> counts(rows, 0);
  std::vector<uint32_t> offsets(rows + 1, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    std::fill(counts.begin(), counts.end(), 0);

    // Count occurrences of each row_id
    // Since row_ids are sorted, this is a linear scan
    for (uint32_t row : row_ids) {
      if (row < rows) {
        counts[row]++;
      }
    }

    // Compute prefix sums for CSR offsets
    uint32_t current_offset = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      current_offset += counts[r];
      offsets[r + 1] = current_offset;
    }

    // Compute checksum
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }

  return final_hash;
}