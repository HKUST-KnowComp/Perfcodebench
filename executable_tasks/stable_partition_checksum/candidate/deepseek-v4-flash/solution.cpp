#include "interface.h"
#include <cstddef>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        hash ^= v[i];
        hash *= 1099511628211ULL;
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      if (!f[i]) {
        hash ^= v[i];
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}