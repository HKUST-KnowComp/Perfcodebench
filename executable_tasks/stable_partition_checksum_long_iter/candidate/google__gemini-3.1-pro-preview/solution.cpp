#include "interface.h"

#include <vector>

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
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  std::size_t n = values.size();
  
  if (n == 0) {
    return hash;
  }

  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  // First pass: hash elements where flag is true (simulating the 'left' partition)
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, v[i]);
    }
  }

  // Second pass: hash elements where flag is false (simulating the 'right' partition)
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, v[i]);
    }
  }

  return hash;
}
