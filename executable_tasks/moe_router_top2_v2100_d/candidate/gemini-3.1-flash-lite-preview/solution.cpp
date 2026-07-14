#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = &data[static_cast<size_t>(t) * experts];
      
      int32_t max1 = -2147483648;
      int idx1 = -1;
      int32_t max2 = -2147483648;
      int idx2 = -1;

      for (int e = 0; e < experts; ++e) {
        int32_t val = row[e];
        if (val > max1 || (val == max1 && e < idx1)) {
          max2 = max1;
          idx2 = idx1;
          max1 = val;
          idx1 = e;
        } else if (val > max2 || (val == max2 && e < idx2)) {
          max2 = val;
          idx2 = e;
        }
      }

      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(max1));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(idx1);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(max2));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(idx2);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}