#include "interface.h"

#include <unordered_map>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::unordered_map<uint32_t, uint32_t> counts;
  std::vector<uint32_t> offsets;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    counts.clear();
    for (uint32_t row : row_ids) {
      ++counts[row];
    }
    offsets.assign(static_cast<std::size_t>(rows) + 1u, 0);
    for (uint32_t r = 0; r < rows; ++r) {
      const auto it = counts.find(r);
      offsets[static_cast<std::size_t>(r + 1u)] =
          offsets[static_cast<std::size_t>(r)] + (it == counts.end() ? 0u : it->second);
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
