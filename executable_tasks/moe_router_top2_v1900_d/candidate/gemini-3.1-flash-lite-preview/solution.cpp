#include "interface.h"
#include <cstdint>
#include <vector>
#include <limits>

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
      const int32_t* row_ptr = &logits[static_cast<size_t>(t) * experts];
      
      int32_t max1 = std::numeric_limits<int32_t>::min();
      int idx1 = -1;
      int32_t max2 = std::numeric_limits<int32_t>::min();
      int idx2 = -1;

      for (int e = 0; e < experts; ++e) {
        int32_t val = row_ptr[e];
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
      
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1)));
      hash = mix(hash, static_cast<uint64_t>(idx1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2)));
      hash = mix(hash, static_cast<uint64_t>(idx2));
    }
  }
  return hash;
}