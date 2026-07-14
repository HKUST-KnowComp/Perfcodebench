#include "interface.h"
#include <cstddef>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  uint64_t result = 0;
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kOffset;
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        h ^= v[i];
        h *= kPrime;
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      if (!f[i]) {
        h ^= v[i];
        h *= kPrime;
      }
    }
    result = h;
  }
  return result;
}