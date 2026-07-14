#include "interface.h"

#include <vector>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  if (n == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  const uint8_t* __restrict__ f = flags.data();
  const uint32_t* __restrict__ v = values.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // First pass: hash all elements where flag is true (left partition)
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        hash = mix(hash, v[i]);
      }
    }
    
    // Second pass: hash all elements where flag is false (right partition)
    for (std::size_t i = 0; i < n; ++i) {
      if (!f[i]) {
        hash = mix(hash, v[i]);
      }
    }
  }
  return hash;
}