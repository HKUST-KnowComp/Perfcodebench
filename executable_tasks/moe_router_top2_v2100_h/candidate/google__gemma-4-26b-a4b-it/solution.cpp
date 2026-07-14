#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = logits_ptr + (static_cast<size_t>(t) * experts);
      
      int32_t max1_val = -2147483648;
      int max1_idx = 0;
      int32_t max2_val = -2147483648;
      int max2_idx = 0;

      for (int e = 0; e < experts; ++e) {
        int32_t val = row_ptr[e];
        // Tie-breaking: higher value, then lower index.
        // We check if current is better than max1, or better than max2.
        if (val > max1_val || (val == max1_val && e < max1_idx)) {
          max2_val = max1_val;
          max2_idx = max1_idx;
          max1_val = val;
          max1_idx = e;
        } else if (val > max2_val || (val == max2_val && e < max2_idx)) {
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
