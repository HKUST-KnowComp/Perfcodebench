#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  uint64_t hash = 0;
  std::vector<uint32_t> offsets;
  offsets.reserve(rows + 1);
  for (int iter = 0; iter < iters; ++iter) {
    offsets.assign(static_cast<std::size_t>(rows) + 1u, 0);
    if (row_ids.empty()) {
      // All offsets remain 0
    } else {
      uint32_t current_row = row_ids[0];
      uint32_t count = 0;
      for (uint32_t row : row_ids) {
        if (row == current_row) {
          count++;
        } else {
          offsets[current_row + 1] = offsets[current_row] + count;
          current_row = row;
          count = 1;
        }
      }
      offsets[current_row + 1] = offsets[current_row] + count;
      for (uint32_t r = current_row + 1; r < rows; ++r) {
        offsets[r + 1] = offsets[r];
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}