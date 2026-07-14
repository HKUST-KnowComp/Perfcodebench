#include "interface.h"

#include <vector>
#include <cstdint>

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
  const size_t n = values.size();
  const uint8_t* f_ptr = flags.data();
  const uint32_t* v_ptr = values.data();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    // First pass: elements where flag is true (left side of partition)
    for (size_t i = 0; i < n; ++i) {
      if (f_ptr[i]) {
        hash = mix(hash, v_ptr[i]);
      }
    }
    
    // Second pass: elements where flag is false (right side of partition)
    for (size_t i = 0; i < n; ++i) {
      if (!f_ptr[i]) {
        hash = mix(hash, v_ptr[i]);
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}