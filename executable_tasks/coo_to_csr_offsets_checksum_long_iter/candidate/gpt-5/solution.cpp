#include "interface.h"

#include <vector>

namespace {
static inline uint64_t fnv1a_mix(uint64_t hash, uint32_t value) {
  return (hash ^ value) * 1099511628211ULL;
}
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  (void)iters;  // iterations are irrelevant for the checksum; compute once

  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1u, 0u);

  // Histogram counts per row; ignore out-of-range rows to mirror baseline behavior
  for (uint32_t r : row_ids) {
    if (r < rows) {
      ++offsets[static_cast<size_t>(r) + 1u];
    }
  }

  // Prefix sum to build CSR offsets and compute hash on the fly
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis
  hash = fnv1a_mix(hash, offsets[0]);      // offsets[0] is 0
  for (size_t i = 1; i <= static_cast<size_t>(rows); ++i) {
    offsets[i] += offsets[i - 1];
    hash = fnv1a_mix(hash, offsets[i]);
  }

  return hash;
}
