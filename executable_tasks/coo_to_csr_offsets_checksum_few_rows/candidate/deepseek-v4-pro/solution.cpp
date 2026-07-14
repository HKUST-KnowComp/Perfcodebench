#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Count occurrences of each row using a direct array (assumes all row_ids < rows)
  std::vector<uint32_t> counts(rows, 0);
  for (uint32_t row : row_ids) {
    // row is guaranteed to be < rows by problem contract
    counts[row]++;
  }

  // Build the CSR offsets array
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u);
  offsets[0] = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    offsets[static_cast<std::size_t>(r) + 1u] = offsets[r] + counts[r];
  }

  // Repeatedly hash the same offsets iters times (the result is independent of iters)
  uint64_t hash = 0;
  const uint64_t seed = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }

  return hash;
}