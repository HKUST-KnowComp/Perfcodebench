#include "interface.h"

#include <vector>
#include <cstdint>
#include <limits>

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
      
      // We need the top 2 experts based on: 
      // 1. Higher logit value
      // 2. Lower index (tie-breaker)
      
      int32_t max1_val = std::numeric_limits<int32_t>::min();
      int max1_idx = 0;
      int32_t max2_val = std::numeric_limits<int32_t>::min();
      int max2_idx = 0;

      // Initialize with first element to handle edge cases correctly
      max1_val = row_ptr[0];
      max1_idx = 0;

      for (int e = 1; e < experts; ++e) {
        int32_t val = row_ptr[e];
        // Comparison logic: better(a, b) is a.first > b.first || (a.first == b.first && a.second < b.second)
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

      // If experts < 2, the logic above might need adjustment, but task implies top-2 exists.
      // The baseline uses row[0] and row[1] after sorting, so we follow that.
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1_val)));
      hash = mix(hash, static_cast<uint64_t>(max1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2_val)));
      hash = mix(hash, static_cast<uint64_t>(max2_idx));
    }
  }
  return hash;
}
