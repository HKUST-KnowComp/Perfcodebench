#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Returns true if (av, ai) is better than (bv, bi)
// Better means higher value; ties broken by smaller index
inline bool better(int32_t av, int ai, int32_t bv, int bi) {
  return av > bv || (av == bv && ai < bi);
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  const std::size_t stride = static_cast<std::size_t>(experts);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const int32_t* row = data;
    for (int t = 0; t < tokens; ++t, row += stride) {
      int e0 = 0, e1 = 1;
      int32_t v0 = row[0], v1 = row[1];
      if (!better(v0, e0, v1, e1)) {
        std::swap(v0, v1);
        std::swap(e0, e1);
      }
      for (int e = 2; e < experts; ++e) {
        int32_t v = row[e];
        if (better(v, e, v0, e0)) {
          v1 = v0;
          e1 = e0;
          v0 = v;
          e0 = e;
        } else if (better(v, e, v1, e1)) {
          v1 = v;
          e1 = e;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v0)));
      hash = mix(hash, static_cast<uint64_t>(e0));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(v1)));
      hash = mix(hash, static_cast<uint64_t>(e1));
    }
  }
  return hash;
}
