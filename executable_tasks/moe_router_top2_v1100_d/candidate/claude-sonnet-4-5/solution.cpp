#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = &logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts)];
      
      // Find top-1
      int32_t max1_val = row_ptr[0];
      int max1_idx = 0;
      
      for (int e = 1; e < experts; ++e) {
        int32_t val = row_ptr[e];
        if (val > max1_val || (val == max1_val && e < max1_idx)) {
          max1_val = val;
          max1_idx = e;
        }
      }
      
      // Find top-2 (excluding top-1)
      int32_t max2_val;
      int max2_idx;
      
      if (max1_idx == 0) {
        max2_val = row_ptr[1];
        max2_idx = 1;
      } else {
        max2_val = row_ptr[0];
        max2_idx = 0;
      }
      
      for (int e = 0; e < experts; ++e) {
        if (e == max1_idx) continue;
        int32_t val = row_ptr[e];
        if (val > max2_val || (val == max2_val && e < max2_idx)) {
          max2_val = val;
          max2_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1_val)));
      hash = mix(hash, static_cast<uint64_t>(max1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2_val)));
      hash = mix(hash, static_cast<uint64_t>(max2_idx));
    }
  }
  
  return hash;
}