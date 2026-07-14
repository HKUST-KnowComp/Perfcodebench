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
  if (iters <= 0) return 0;

  // Pre-allocate offsets vector to avoid repeated allocations
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Since row_ids are sorted, we can compute offsets in O(N + rows)
    // Reset offsets
    std::fill(offsets.begin(), offsets.end(), 0);

    // Count occurrences of each row id
    // Because row_ids is sorted, we can just iterate and increment
    for (uint32_t row : row_ids) {
      offsets[row + 1]++;
    }

    // Prefix sum to convert counts to offsets
    uint32_t current_offset = 0;
    for (uint32_t i = 0; i < rows; ++i) {
      uint32_t count = offsets[i + 1];
      current_offset += count;
      offsets[i + 1] = current_offset;
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