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
  const size_t n = row_ids.size();
  const size_t offset_size = static_cast<size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(offset_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Zero out offsets
    for (size_t i = 0; i < offset_size; ++i) {
      offsets[i] = 0;
    }

    // Count occurrences of each row (store counts in offsets[row+1])
    for (size_t i = 0; i < n; ++i) {
      uint32_t row = row_ids[i];
      ++offsets[static_cast<size_t>(row) + 1u];
    }

    // Prefix sum to convert counts to offsets
    for (size_t i = 1; i < offset_size; ++i) {
      offsets[i] += offsets[i - 1];
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < offset_size; ++i) {
      hash = mix(hash, offsets[i]);
    }
  }

  return hash;
}
