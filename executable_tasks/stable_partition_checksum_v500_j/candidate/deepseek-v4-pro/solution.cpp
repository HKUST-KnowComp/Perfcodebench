#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t kInitialHash = 1469598103934665603ULL;
constexpr uint64_t kHashMultiplier = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
  return (hash ^ value) * kHashMultiplier;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kInitialHash;
    // Process 'true' group first (stable order)
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i]) {
        hash = mix(hash, v[i]);
      }
    }
    // Process 'false' group second
    for (std::size_t i = 0; i < n; ++i) {
      if (!f[i]) {
        hash = mix(hash, v[i]);
      }
    }
  }
  return hash;
}