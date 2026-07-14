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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint32_t> touched;
  touched.reserve(group_count);
  
  uint64_t hash = 0;
  const std::size_t n = groups.size();
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  uint32_t* __restrict b_ptr = best.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear only touched entries
    for (uint32_t idx : touched) {
      b_ptr[idx] = 0;
    }
    touched.clear();
    
    // Find max per group
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t gid = g_ptr[i];
      uint32_t val = v_ptr[i];
      if (val > b_ptr[gid]) {
        if (b_ptr[gid] == 0 && val > 0) {
          touched.push_back(gid);
        }
        b_ptr[gid] = val;
      } else if (b_ptr[gid] == 0 && val == 0) {
        // Group seen with value 0, still need to track it was touched
        // but only add once
      }
    }
    
    // Compute hash over all groups in order
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
  }
  return hash;
}