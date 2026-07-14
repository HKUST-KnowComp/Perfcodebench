#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * experts;
      
      int32_t v0 = row[0];
      int32_t v1 = row[1];
      int32_t best_val, second_val;
      int best_idx, second_idx;
      
      if (v0 >= v1) {
        best_val = v0; best_idx = 0;
        second_val = v1; second_idx = 1;
      } else {
        best_val = v1; best_idx = 1;
        second_val = v0; second_idx = 0;
      }
      
      for (int e = 2; e < experts; ++e) {
        int32_t val = row[e];
        if (val > best_val) {
          second_val = best_val; second_idx = best_idx;
          best_val = val; best_idx = e;
        } else if (val > second_val) {
          second_val = val; second_idx = e;
        }
      }
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
      hash = mix(hash, static_cast<uint64_t>(best_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
      hash = mix(hash, static_cast<uint64_t>(second_idx));
    }
  }
  return hash;
}