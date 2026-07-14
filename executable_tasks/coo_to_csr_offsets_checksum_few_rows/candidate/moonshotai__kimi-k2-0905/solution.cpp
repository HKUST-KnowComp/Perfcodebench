#include "interface.h"
#include <cstring>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  static thread_local std::vector<uint32_t> counts;
  static thread_local std::vector<uint32_t> offsets;

  if (counts.size() < static_cast<size_t>(rows)) {
    counts.resize(static_cast<size_t>(rows));
  }
  if (offsets.size() < static_cast<size_t>(rows) + 1) {
    offsets.resize(static_cast<size_t>(rows) + 1);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts.data(), 0, sizeof(uint32_t) * rows);

    for (uint32_t row : row_ids) {
      ++counts[row];
    }

    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      offsets[r + 1] = offsets[r] + counts[r];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
  }
  return hash;
}