#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  if (iters <= 0) return 0;
  const size_t N = indices.size();
  std::vector<uint64_t> accum(domain, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate
    for (size_t i = 0; i < N; ++i) {
      accum[indices[i]] += static_cast<uint64_t>(values[i]);
    }
    // Compute hash and reset accum
    uint64_t h = 1469598103934665603ULL;
    uint64_t* data = accum.data();
    for (uint32_t key = 0; key < domain; ++key) {
      uint64_t v = data[key];
      data[key] = 0;
      h ^= v;
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}