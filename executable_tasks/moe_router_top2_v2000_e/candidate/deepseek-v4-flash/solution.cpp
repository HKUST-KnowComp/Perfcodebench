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

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  // Cache top-2 (logit, index) for each token.
  std::vector<int32_t> top1_logit(tokens);
  std::vector<int32_t> top2_logit(tokens);
  std::vector<int> top1_idx(tokens);
  std::vector<int> top2_idx(tokens);

  for (int t = 0; t < tokens; ++t) {
    const size_t base = static_cast<size_t>(t) * experts;

    // Find top-1
    int32_t best_logit = INT32_MIN;
    int best_idx = 0;
    for (int e = 0; e < experts; ++e) {
      int32_t val = logits[base + e];
      if (val > best_logit || (val == best_logit && e < best_idx)) {
        best_logit = val;
        best_idx = e;
      }
    }

    // Find top-2 (excluding first best)
    int32_t second_logit = INT32_MIN;
    int second_idx = 0;
    for (int e = 0; e < experts; ++e) {
      if (e == best_idx) continue;
      int32_t val = logits[base + e];
      if (val > second_logit || (val == second_logit && e < second_idx)) {
        second_logit = val;
        second_idx = e;
      }
    }

    top1_logit[t] = best_logit;
    top1_idx[t]   = best_idx;
    top2_logit[t] = second_logit;
    top2_idx[t]   = second_idx;
  }

  // Compute checksum over cached results, repeating iters times.
  uint64_t hash = 0;
  const uint64_t offset = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = offset;
    for (int t = 0; t < tokens; ++t) {
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_logit[t])));
      hash = mix(hash, static_cast<uint64_t>(top1_idx[t]));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_logit[t])));
      hash = mix(hash, static_cast<uint64_t>(top2_idx[t]));
    }
  }
  return hash;
}
