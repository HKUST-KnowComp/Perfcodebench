#include "interface.h"

#include <cstdint>
#include <vector>
#include <climits>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

inline bool is_better(int32_t cand_v, int cand_e, int32_t cur_v, int cur_e) {
  return cand_v > cur_v || (cand_v == cur_v && cand_e < cur_e);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t final_hash = 0;
  const size_t experts_sz = static_cast<size_t>(experts);
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const size_t token_offset = static_cast<size_t>(t) * experts_sz;
      int32_t max1_v = INT32_MIN;
      int max1_e = INT_MAX;
      int32_t max2_v = INT32_MIN;
      int max2_e = INT_MAX;

      for (int e = 0; e < experts; ++e) {
        const int32_t v = logits[token_offset + static_cast<size_t>(e)];
        if (is_better(v, e, max1_v, max1_e)) {
          max2_v = max1_v;
          max2_e = max1_e;
          max1_v = v;
          max1_e = e;
        } else if (is_better(v, e, max2_v, max2_e)) {
          max2_v = v;
          max2_e = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1_v)));
      hash = mix(hash, static_cast<uint64_t>(max1_e));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2_v)));
      hash = mix(hash, static_cast<uint64_t>(max2_e));
    }
    final_hash = hash;
  }
  return final_hash;
}