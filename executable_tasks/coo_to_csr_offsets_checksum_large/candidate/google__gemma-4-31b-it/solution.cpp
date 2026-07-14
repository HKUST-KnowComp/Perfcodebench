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
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (size_t i = 0; i < offsets.size(); ++i) offsets[i] = 0;

    // Since row_ids are sorted, we can count occurrences efficiently
    // The number of elements in row r is the count of r in row_ids
    // We can simply iterate through row_ids and increment the count for that row
    for (uint32_t row : row_ids) {
      if (row < rows) {
        offsets[row + 1]++;
      }
    }

    // Compute prefix sums to get CSR offsets
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] += offsets[r];
    }

    // Calculate checksum
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}