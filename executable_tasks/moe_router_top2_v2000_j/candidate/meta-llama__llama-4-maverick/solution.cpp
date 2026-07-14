#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

void partial_sort_top2(const int32_t* logits, int experts, int32_t& max1, int& idx1, int32_t& max2, int& idx2) {
  max1 = logits[0];
  idx1 = 0;
  max2 = logits[0];
  idx2 = 0;
  for (int e = 1; e < experts; ++e) {
    int32_t val = logits[e];
    int idx = e;
    if (val > max1 || (val == max1 && idx < idx1)) {
      max2 = max1;
      idx2 = idx1;
      max1 = val;
      idx1 = idx;
    } else if (val > max2 || (val == max2 && idx < idx2)) {
      max2 = val;
      idx2 = idx;
    }
  }
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row_logits = logits.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
      int32_t max1, max2;
      int idx1, idx2;
      partial_sort_top2(row_logits, experts, max1, idx1, max2, idx2);
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1)));
      hash = mix(hash, static_cast<uint64_t>(idx1));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2)));
      hash = mix(hash, static_cast<uint64_t>(idx2));
    }
  }
  return hash;
}
