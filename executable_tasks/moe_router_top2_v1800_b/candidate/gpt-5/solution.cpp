#include "interface.h"

#include <cstdint>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  const std::size_t stride = static_cast<std::size_t>(experts);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = logits.data() + static_cast<std::size_t>(t) * stride;

      int32_t best1v = INT32_MIN;
      int best1i = INT32_MAX;
      int32_t best2v = INT32_MIN;
      int best2i = INT32_MAX;

      for (int e = 0; e < experts; ++e) {
        const int32_t v = row[static_cast<std::size_t>(e)];
        if (v > best1v || (v == best1v && e < best1i)) {
          // Promote current best1 to best2, and set new best1
          best2v = best1v;
          best2i = best1i;
          best1v = v;
          best1i = e;
        } else if (v > best2v || (v == best2v && e < best2i)) {
          best2v = v;
          best2i = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best1v)));
      hash = mix(hash, static_cast<uint64_t>(best1i));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best2v)));
      hash = mix(hash, static_cast<uint64_t>(best2i));
    }
  }

  return hash;
}
