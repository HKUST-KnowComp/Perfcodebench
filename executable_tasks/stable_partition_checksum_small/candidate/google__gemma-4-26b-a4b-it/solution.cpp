#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (values.empty()) {
    return 0;
  }

  const size_t n = values.size();
  const uint32_t* val_ptr = values.data();
  const uint8_t* flag_ptr = flags.data();
  
  uint64_t hash = 1469598103934665603ULL;

  // The hash is recalculated 'iters' times, but since the input is constant,
  // the result is the same for every iteration. We only need to compute it once.
  // However, to strictly follow the logic of the original loop structure:
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // First pass: elements where flag is true (the 'left' part of stable partition)
    for (size_t i = 0; i < n; ++i) {
      if (flag_ptr[i]) {
        hash = mix(hash, val_ptr[i]);
      }
    }
    
    // Second pass: elements where flag is false (the 'right' part of stable partition)
    for (size_t i = 0; i < n; ++i) {
      if (!flag_ptr[i]) {
        hash = mix(hash, val_ptr[i]);
      }
    }
  }

  return hash;
}