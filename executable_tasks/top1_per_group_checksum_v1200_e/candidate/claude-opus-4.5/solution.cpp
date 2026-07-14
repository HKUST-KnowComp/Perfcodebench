#include "interface.h"

#include <cstring>

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
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  const size_t n = groups.size();
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(best.data(), 0, group_count * sizeof(uint32_t));
    uint32_t* __restrict b_ptr = best.data();
    
    size_t i = 0;
    // Process 4 elements at a time
    for (; i + 4 <= n; i += 4) {
      uint32_t g0 = g_ptr[i];
      uint32_t g1 = g_ptr[i + 1];
      uint32_t g2 = g_ptr[i + 2];
      uint32_t g3 = g_ptr[i + 3];
      uint32_t v0 = v_ptr[i];
      uint32_t v1 = v_ptr[i + 1];
      uint32_t v2 = v_ptr[i + 2];
      uint32_t v3 = v_ptr[i + 3];
      
      if (v0 > b_ptr[g0]) b_ptr[g0] = v0;
      if (v1 > b_ptr[g1]) b_ptr[g1] = v1;
      if (v2 > b_ptr[g2]) b_ptr[g2] = v2;
      if (v3 > b_ptr[g3]) b_ptr[g3] = v3;
    }
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > b_ptr[g]) b_ptr[g] = v;
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
  }
  return hash;
}