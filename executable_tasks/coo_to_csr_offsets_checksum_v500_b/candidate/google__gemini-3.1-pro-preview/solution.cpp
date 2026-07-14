#include "interface.h"
#include <vector>
#include <cstdint>
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

  // Pre-allocate buffers to avoid repeated allocations inside the loop.
  // offsets size is rows + 1.
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets to zero
    std::fill(offsets.begin(), offsets.end(), 0);

    // Count frequencies of each row_id.
    // Since row_ids are sorted in standard COO-to-CSR, we could optimize further,
    // but a simple frequency count is robust and fast.
    for (uint32_t row : row_ids) {
      if (row < rows) {
        offsets[row + 1]++;
      }
    }

    // Prefix sum to transform counts into offsets.
    // offsets[0] is always 0. offsets[i+1] = sum(counts[0...i])
    uint32_t current_offset = 0;
    for (uint32_t i = 1; i <= rows; ++i) {
      current_offset += offsets[i];
      offsets[i] = current_offset;
    }

    // Compute the hash of the offsets array.
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }

  return final_hash;
}