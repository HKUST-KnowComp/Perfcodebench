#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      int best1_idx = 0;
      int32_t best1_val = row[0];
      int best2_idx = -1;
      int32_t best2_val = 0;
      for (int e = 1; e < experts; ++e) {
        int32_t val = row[e];
        if (val > best1_val || (val == best1_val && e < best1_idx)) {
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = val;
          best1_idx = e;
        } else if (best2_idx == -1 || val > best2_val || (val == best2_val && e < best2_idx)) {
          best2_val = val;
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