#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  if (iters == 0) {
    return 0ULL;
  }
  // Accumulate once (sparse updates into dense vector)
  std::vector<uint64_t> accum(domain, 0ULL);
  for (std::size_t i = 0; i < indices.size(); ++i) {
    accum[indices[i]] += static_cast<uint64_t>(values[i]);
  }
  // Compute FNV-1a checksum over the entire domain
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t key = 0; key < domain; ++key) {
    hash ^= accum[key];
    hash *= 1099511628211ULL;
  }
  return hash;
}
