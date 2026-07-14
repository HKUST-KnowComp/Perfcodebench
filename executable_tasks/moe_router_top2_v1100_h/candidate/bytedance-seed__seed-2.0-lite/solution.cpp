#include "interface.h"

#include <cstdint>
#include <climits>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
      int32_t max1 = INT32_MIN;
      int idx1 = -1;
      int32_t max2 = INT32_MIN;
      int idx2 = -1;
      const std::size_t token_offset = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      for (int e = 0; e < experts; ++e) {
        const int32_t val = logits[token_offset + static_cast<std::size_t>(e)];
        if ((val > max1) || (val == max1 && e < idx1)) {
          max2 = max1;
          idx2 = idx1;
          max1 = val;
          idx1 = e;
        } else if ((val > max2) || (val == max2 && e < idx2)) {
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