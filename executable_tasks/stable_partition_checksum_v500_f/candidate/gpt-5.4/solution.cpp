#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (n == 0 || iters <= 0) {
    return 0;
  }

  std::size_t left_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    left_count += (flags[i] != 0);
  }

  std::vector<uint32_t> out(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t l = 0;
    std::size_t r = left_count;

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t v = values[i];
      if (flags[i]) {
        out[l++] = v;
      } else {
        out[r++] = v;
      }
    }

    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      h ^= out[i];
      h *= 1099511628211ULL;
    }
    hash = h;
  }

  return hash;
}
