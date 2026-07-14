#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Dense accumulator: one entry per possible index.
  std::vector<uint64_t> accum(domain, 0ULL);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate sparse updates.
    for (std::size_t i = 0; i < indices.size(); ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }

    // Compute FNV-1a checksum over the entire domain.
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }

    // Reset only the touched indices for the next iteration.
    for (std::size_t i = 0; i < indices.size(); ++i) {
      accum[indices[i]] = 0ULL;
    }
  }

  return hash;
}
