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
  if (experts < 2) return 0; // Based on task, experts >= 2 is expected for top-2

  uint64_t final_hash = 0;
  const int32_t* data_ptr = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_ptr = data_ptr + (static_cast<size_t>(t) * experts);
      
      int32_t max1_val = row_ptr[0];
      int max1_idx = 0;
      int32_t max2_val = -2147483648;
      int max2_idx = -1;

      for (int e = 1; e < experts; ++e) {
        int32_t val = row_ptr[e];
        if (val > max1_val) {
          max2_val = max1_val;
          max2_idx = max1_idx;
          max1_val = val;
          max1_idx = e;
        } else if (val == max1_val) {
          // Tie-break: lower index is better. 
          // Since we iterate e from 1 to experts-1, max1_idx is already the lowest index for max1_val.
          // We only check if this current e is better than max2.
          if (max2_idx == -1 || val > max2_val) {
            max2_val = val;
            max2_idx = e;
          } else if (val == max2_val) {
            // max2_idx is already the lowest index for max2_val
          }
        } else if (max2_idx == -1 || val > max2_val) {
          max2_val = val;
          max2_idx = e;
        } else if (val == max2_val) {
          // max2_idx is already the lowest index
        }
      }

      // Correct tie-breaking logic for top-2:
      // We need the two elements that would be at index 0 and 1 after sorting by (val DESC, idx ASC).
      // The loop above handles max1 correctly. For max2, if val == max1_val, the first encountered 
      // index after max1_idx is the correct second best.
      
      // Re-evaluating the linear scan for strict tie-breaking:
      // Let's use a simpler, robust approach for top-2 selection.
      int32_t v1 = row_ptr[0], v2 = -2147483648;
      int i1 = 0, i2 = -1;
      for (int e = 1; e < experts; ++e) {
        int32_t v = row_ptr[e];
        if (v > v1) {
          v2 = v1; i2 = i1;
          v1 = v; i1 = e;
        } else if (v == v1) {
          if (i2 == -1 || v > v2) {
            v2 = v; i2 = e;
          } else if (v == v2) {
            // i2 is already smaller than e
          }
        } else {
          if (i2 == -1 || v > v2) {
            v2 = v; i2 = e;
          } else if (v == v2) {
            // i2 is already smaller than e
          }
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v1)));
      hash = mix(hash, static_cast<uint64_t>(i1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v2)));
      hash = mix(hash, static_cast<uint64_t>(i2));
    }
    final_hash = hash;
  }
  return final_hash;
}
