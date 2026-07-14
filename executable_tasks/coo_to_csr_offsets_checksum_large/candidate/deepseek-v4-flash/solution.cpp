#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  // Compute CSR row offsets from sorted COO row IDs in one pass.
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1, 0);
  std::size_t current_row = 0;
  std::size_t count = 0;
  for (std::size_t i = 0; i < row_ids.size(); ++i) {
    uint32_t r = row_ids[i];
    while (current_row < r) {
      offsets[current_row + 1] = offsets[current_row] + static_cast<uint32_t>(count);
      count = 0;
      ++current_row;
    }
    ++count;
  }
  // Fill remaining rows up to 'rows'.
  while (current_row < rows) {
    offsets[current_row + 1] = offsets[current_row] + static_cast<uint32_t>(count);
    count = 0;
    ++current_row;
  }

  // Compute the FNV-1a hash of the offsets array, repeated 'iters' times.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
