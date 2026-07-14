#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        h ^= v[i];
        h *= 1099511628211ULL;
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      if (!f[i]) {
        h ^= v[i];
        h *= 1099511628211ULL;
      }
    }
    hash = h;
  }
  return hash;
}