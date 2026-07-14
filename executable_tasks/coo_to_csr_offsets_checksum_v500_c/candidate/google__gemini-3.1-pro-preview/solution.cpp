#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

/**
 * The mix function provided in the baseline.
 * It is a variant of the FNV-1a hash step.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

/**
 * Computes the CSR offset checksum for a single iteration.
 * Since row_ids is sorted, offsets[r+1] is the index of the first element 
 * in row_ids that is >= r+1.
 */
uint64_t compute_hash(const std::vector<uint32_t>& row_ids, uint32_t rows) {
  const uint32_t n = static_cast<uint32_t>(row_ids.size());
  uint64_t hash = 1469598103934665603ULL;

  // offsets[0] is always 0
  hash = mix(hash, 0);

  uint32_t last_r = 0;
  const uint32_t* __restrict__ data = row_ids.data();

  // Iterate through sorted row_ids to find transitions
  for (uint32_t i = 0; i < n; ++i) {
    uint32_t r = data[i];
    if (r > last_r) {
      // All rows from last_r to r-1 have the same offset (the current index i)
      // This corresponds to offsets[last_r + 1] ... offsets[r]
      uint32_t count = r - last_r;
      uint64_t val = static_cast<uint64_t>(i);
      for (uint32_t j = 0; j < count; ++j) {
        hash = mix(hash, val);
      }
      last_r = r;
    }
  }

  // All remaining rows from last_r to rows-1 have the same offset (n)
  // This corresponds to offsets[last_r + 1] ... offsets[rows]
  if (rows > last_r) {
    uint32_t count = rows - last_r;
    uint64_t val = static_cast<uint64_t>(n);
    for (uint32_t j = 0; j < count; ++j) {
      hash = mix(hash, val);
    }
  }

  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 0;
  // The problem asks for the checksum of the offset array returned in the last iteration.
  // Since the input row_ids and rows do not change, the hash is identical for every iteration.
  for (int iter = 0; iter < iters; ++iter) {
    hash = compute_hash(row_ids, rows);
  }

  return hash;
}
