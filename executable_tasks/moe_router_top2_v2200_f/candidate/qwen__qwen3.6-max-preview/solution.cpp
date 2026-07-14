#include "interface.h"

#include <cstdint>
#include <vector>
#include <climits>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    const int32_t* row = data;
    for (int t = 0; t < tokens; ++t) {
      int32_t v1 = INT32_MIN;
      int idx1 = experts;
      int32_t v2 = INT32_MIN;
      int idx2 = experts;
      
      for (int e = 0; e < experts; ++e) {
        int32_t v = row[e];
        if (v > v1 || (v == v1 && e < idx1)) {
          v2 = v1; idx2 = idx1;
          v1 = v; idx1 = e;
        } else if (v > v2 || (v == v2 && e < idx2)) {
          v2 = v; idx2 = e;
        }
      }
      row += experts;
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v1)));
      hash = mix(hash, static_cast<uint64_t>(idx1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v2)));
      hash = mix(hash, static_cast<uint64_t>(idx2));
    }
  }
  return hash;
}
