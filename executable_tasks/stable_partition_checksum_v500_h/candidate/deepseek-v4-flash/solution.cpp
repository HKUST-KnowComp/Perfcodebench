#include "interface.h"
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (flags[i]) {
        hash ^= values[i];
        hash *= 1099511628211ULL;
      }
    }
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (!flags[i]) {
        hash ^= values[i];
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}