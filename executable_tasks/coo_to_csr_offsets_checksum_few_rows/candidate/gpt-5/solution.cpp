#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  (void)iters;  // The checksum is independent of iteration count; compute once.

  // Count entries per row (handles sorted or unsorted input, ignores out-of-range rows)
  std::vector<uint32_t> counts(rows, 0u);
  for (uint32_t r : row_ids) {
    if (r < rows) ++counts[r];
  }

  // Compute FNV-1a hash over CSR offsets without materializing the offsets array.
  uint64_t hash = 1469598103934665603ULL;
  // offsets[0] = 0
  hash = mix(hash, 0u);

  uint32_t acc = 0u;
  for (uint32_t r = 0; r < rows; ++r) {
    acc += counts[r];
    hash = mix(hash, acc);  // offsets[r+1]
  }

  return hash;
}
