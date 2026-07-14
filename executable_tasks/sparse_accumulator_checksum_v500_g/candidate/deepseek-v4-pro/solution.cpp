#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  if (domain == 0) {
    // If domain is zero, no keys to iterate, return initial hash unchanged.
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
      (void)indices; // no accumulation needed
      (void)values;
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Persistent accumulator, zero-initialized once.
  std::vector<uint64_t> acc(domain, 0ULL);
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Phase 1: accumulate values
    const std::size_t N = indices.size();
    for (std::size_t i = 0; i < N; ++i) {
      acc[indices[i]] += static_cast<uint64_t>(values[i]);
    }

    // Phase 2: compute checksum over all keys
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, acc[key]);
    }

    // Phase 3: reset only the entries that were touched
    for (std::size_t i = 0; i < N; ++i) {
      acc[indices[i]] = 0ULL;
    }
  }

  return hash;
}
