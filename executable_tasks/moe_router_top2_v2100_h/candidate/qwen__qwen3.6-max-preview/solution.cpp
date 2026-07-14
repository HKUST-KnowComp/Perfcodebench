#include "interface.h"
#include <cstdint>
#include <vector>
#include <climits>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const int32_t* ptr = data;
    for (int t = 0; t < tokens; ++t) {
      int32_t v1 = INT32_MIN, v2 = INT32_MIN;
      int i1 = experts, i2 = experts;
      
      for (int e = 0; e < experts; ++e) {
        int32_t v = ptr[e];
        if (v > v1 || (v == v1 && e < i1)) {
          v2 = v1; i2 = i1;
          v1 = v; i1 = e;
        } else if (v > v2 || (v == v2 && e < i2)) {
          v2 = v; i2 = e;
        }
      }
      ptr += experts;
      
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v1));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(i1);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(v2));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(i2);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}