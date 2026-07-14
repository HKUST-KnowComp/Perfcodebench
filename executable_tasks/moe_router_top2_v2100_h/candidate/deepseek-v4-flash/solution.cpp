#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* logits_ptr = logits.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_start = logits_ptr + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);

      // Initialize with first expert
      std::pair<int32_t, int> best = {row_start[0], 0};
      std::pair<int32_t, int> second = {row_start[1], 1};
      if (better(second, best)) {
        std::swap(best, second);
      }

      // Scan remaining experts
      for (int e = 2; e < experts; ++e) {
        std::pair<int32_t, int> cur = {row_start[e], e};
        if (better(cur, best)) {
          second = best;
          best = cur;
        } else if (better(cur, second)) {
          second = cur;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best.first)));
      hash = mix(hash, static_cast<uint64_t>(best.second));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second.first)));
      hash = mix(hash, static_cast<uint64_t>(second.second));
    }
  }
  return hash;
}
