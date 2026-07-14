#include "interface.h"

#include <vector>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
  const size_t N = row_ids.size();
  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  offsets[0] = 0;
  size_t idx = 0;
  for (uint32_t r = 0; r < rows; ++r) {
    uint32_t cnt = 0;
    while (idx < N && row_ids[idx] == r) {
      ++cnt;
      ++idx;
    }
    offsets[static_cast<size_t>(r) + 1] = offsets[static_cast<size_t>(r)] + cnt;
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash = mix(hash, v);
  }
  return hash;
}
