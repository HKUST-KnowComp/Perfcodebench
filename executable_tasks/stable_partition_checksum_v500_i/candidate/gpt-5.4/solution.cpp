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
  uint64_t hash = 0;

  if (iters <= 0) return hash;

  std::size_t left_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    left_count += static_cast<std::size_t>(flags[i] != 0);
  }

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
  }

  (void)left_count;
  return hash;
}
