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
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (uint32_t i = 0; i <= rows; ++i) offsets[i] = 0;

    // Since row_ids are sorted, we can count occurrences efficiently
    // However, the most robust way to build CSR offsets from sorted COO is
    // to count how many elements belong to each row.
    // Given the sorted property, we can just iterate and increment.
    for (uint32_t row : row_ids) {
      if (row < rows) {
        offsets[row + 1]++;
      }
    }

    // Prefix sum to convert counts to offsets
    for (uint32_t i = 0; i < rows; ++i) {
      offsets[i + 1] += offsets[i];
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }
  return final_hash;
}