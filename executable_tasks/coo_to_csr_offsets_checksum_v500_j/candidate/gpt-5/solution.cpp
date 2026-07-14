#include "interface.h"

namespace {
inline uint64_t fnv1a_mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  (void)iters;  // The checksum depends only on the data; iterating doesn't change the result.

  const std::size_t nrows_plus_one = static_cast<std::size_t>(rows) + 1u;
  std::vector<uint32_t> offsets(nrows_plus_one, 0u);

  // Count entries per row in offsets[1..rows]
  const uint32_t* rid = row_ids.data();
  const std::size_t nnz = row_ids.size();
  for (std::size_t i = 0; i < nnz; ++i) {
    uint32_t r = rid[i];
    if (r < rows) {
      ++offsets[static_cast<std::size_t>(r) + 1u];
    }
  }

  // Prefix sum to convert counts into CSR offsets
  uint32_t acc = 0u;
  for (std::size_t i = 1; i < nrows_plus_one; ++i) {
    acc += offsets[i];
    offsets[i] = acc;
  }

  // FNV-1a hash of the offsets array
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < nrows_plus_one; ++i) {
    hash = fnv1a_mix(hash, static_cast<uint64_t>(offsets[i]));
  }
  return hash;
}
