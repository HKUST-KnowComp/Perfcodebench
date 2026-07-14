#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(offsets.begin(), offsets.end(), 0u);

    if (!row_ids.empty()) {
      uint32_t current_row = row_ids[0];
      uint32_t count = 1;
      for (size_t i = 1; i < row_ids.size(); ++i) {
        if (row_ids[i] == current_row) {
          ++count;
        } else {
          offsets[static_cast<size_t>(current_row) + 1u] += count;
          current_row = row_ids[i];
          count = 1;
        }
      }
      offsets[static_cast<size_t>(current_row) + 1u] += count;
    }

    for (uint32_t r = 0; r < rows; ++r) {
      offsets[static_cast<size_t>(r + 1u)] += offsets[static_cast<size_t>(r)];
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    final_hash = hash;
  }

  return final_hash;
}