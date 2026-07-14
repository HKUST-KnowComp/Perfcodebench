#include "interface.h"

namespace {

inline bool better_pair(int32_t va, int ia, int32_t vb, int ib) {
  return (va > vb) || (va == vb && ia < ib);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  if (iters <= 0) return 0ULL;

  const int32_t* data = logits.data();
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  for (int t = 0; t < tokens; ++t) {
    const std::size_t base = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);
    const int32_t* row = data + base;

    // Assume experts >= 2 as per task usage
    int32_t best_val = row[0];
    int best_idx = 0;
    int32_t second_val = row[1];
    int second_idx = 1;

    if (better_pair(second_val, second_idx, best_val, best_idx)) {
      int32_t tv = best_val; best_val = second_val; second_val = tv;
      int ti = best_idx; best_idx = second_idx; second_idx = ti;
    }

    for (int e = 2; e < experts; ++e) {
      int32_t v = row[e];
      int idx = e;
      if (better_pair(v, idx, best_val, best_idx)) {
        // promote current best to second
        second_val = best_val; second_idx = best_idx;
        best_val = v; best_idx = idx;
      } else if (better_pair(v, idx, second_val, second_idx)) {
        second_val = v; second_idx = idx;
      }
    }

    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best_val)));
    hash = mix(hash, static_cast<uint64_t>(best_idx));
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(second_val)));
    hash = mix(hash, static_cast<uint64_t>(second_idx));
  }

  return hash;
}
