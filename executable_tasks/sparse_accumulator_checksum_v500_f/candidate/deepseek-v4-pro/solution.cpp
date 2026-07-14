#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint64_t> accum(domain);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t n = static_cast<uint32_t>(indices.size());
    for (uint32_t i = 0; i < n; ++i) {
      const uint32_t idx = indices[i];
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      hash = mix(hash, accum[key]);
    }
    for (uint32_t i = 0; i < n; ++i) {
      accum[indices[i]] = 0;
    }
  }
  return hash;
}
