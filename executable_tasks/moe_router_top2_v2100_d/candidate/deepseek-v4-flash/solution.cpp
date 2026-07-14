#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  // Precompute the top-2 experts for each token
  std::vector<uint64_t> precomp(static_cast<std::size_t>(tokens) * 4);
  for (int t = 0; t < tokens; ++t) {
    const int32_t* row = &logits[static_cast<std::size_t>(t) * experts];
    int best_idx = 0;
    int second_idx = -1;
    int32_t best_val = row[0];
    int32_t second_val = 0; // placeholder, will be overwritten

    for (int e = 1; e < experts; ++e) {
      int32_t v = row[e];
      // Compare with current best (higher logit wins, tie goes to smaller index)
      if (v > best_val || (v == best_val && e < best_idx)) {
        second_val = best_val;
        second_idx = best_idx;
        best_val = v;
        best_idx = e;
      } else if (second_idx == -1 || v > second_val || (v == second_val && e < second_idx)) {
        second_val = v;
        second_idx = e;
      }
    }

    std::size_t base = static_cast<std::size_t>(t) * 4;
    precomp[base]     = static_cast<uint64_t>(static_cast<uint32_t>(best_val));
    precomp[base + 1] = static_cast<uint64_t>(best_idx);
    precomp[base + 2] = static_cast<uint64_t>(static_cast<uint32_t>(second_val));
    precomp[base + 3] = static_cast<uint64_t>(second_idx);
  }

  // Compute the checksum iters times using the precomputed values
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t idx = 0;
    for (int t = 0; t < tokens; ++t) {
      hash ^= precomp[idx++];
      hash *= 1099511628211ULL;
      hash ^= precomp[idx++];
      hash *= 1099511628211ULL;
      hash ^= precomp[idx++];
      hash *= 1099511628211ULL;
      hash ^= precomp[idx++];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
