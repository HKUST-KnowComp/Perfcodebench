#include "interface.h"

#include <cstdint>
#include <vector>
#include <limits>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const int E = experts;
  const int T = tokens;
  const int32_t* data = logits.data();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t offset = 0;
    for (int t = 0; t < T; ++t) {
      int32_t best1_val = std::numeric_limits<int32_t>::min();
      int best1_idx = -1;
      int32_t best2_val = std::numeric_limits<int32_t>::min();
      int best2_idx = -1;

      const int32_t* row = data + offset;
      for (int e = 0; e < E; ++e) {
        int32_t v = row[e];
        if (v > best1_val || (v == best1_val && e < best1_idx)) {
          best2_val = best1_val;
          best2_idx = best1_idx;
          best1_val = v;
          best1_idx = e;
        } else if (v > best2_val || (v == best2_val && e < best2_idx)) {
          best2_val = v;
          best2_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1_val)));
      hash = mix(hash, static_cast<uint64_t>(best1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2_val)));
      hash = mix(hash, static_cast<uint64_t>(best2_idx));

      offset += static_cast<std::size_t>(E);
    }
  }

  return hash;
}
