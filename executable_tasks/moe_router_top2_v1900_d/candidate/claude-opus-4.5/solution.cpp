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
  const int32_t* base = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = base + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      
      // Find top-1: highest logit, lowest index on tie
      int32_t best1_val = row[0];
      int best1_idx = 0;
      
      for (int e = 1; e < experts; ++e) {
        int32_t v = row[e];
        if (v > best1_val) {
          best1_val = v;
          best1_idx = e;
        }
      }
      
      // Find top-2: highest logit excluding best1_idx, lowest index on tie
      int32_t best2_val;
      int best2_idx;
      
      if (best1_idx == 0) {
        best2_val = row[1];
        best2_idx = 1;
      } else {
        best2_val = row[0];
        best2_idx = 0;
      }
      
      for (int e = best2_idx + 1; e < experts; ++e) {
        if (e == best1_idx) continue;
        int32_t v = row[e];
        if (v > best2_val) {
          best2_val = v;
          best2_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));
    }
  }
  
  return hash;
}