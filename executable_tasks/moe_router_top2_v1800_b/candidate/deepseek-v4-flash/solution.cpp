#include "interface.h"

#include <climits>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffsetBasis;
    for (int t = 0; t < tokens; ++t) {
      const int32_t* row = logits.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);

      // Initialize with first two experts
      int32_t best1_score = row[0];
      int best1_idx = 0;
      int32_t best2_score = row[1];
      int best2_idx = 1;

      // Ensure initial order: best1 better than best2
      if (best1_score < best2_score || (best1_score == best2_score && best1_idx > best2_idx)) {
        std::swap(best1_score, best2_score);
        std::swap(best1_idx, best2_idx);
      }

      for (int e = 2; e < experts; ++e) {
        int32_t score = row[e];
        if (score > best1_score || (score == best1_score && e < best1_idx)) {
          // New best1, shift old best1 to best2
          best2_score = best1_score;
          best2_idx = best1_idx;
          best1_score = score;
          best1_idx = e;
        } else if (score > best2_score || (score == best2_score && e < best2_idx)) {
          // New best2
          best2_score = score;
          best2_idx = e;
        }
      }

      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(best1_score));
      hash *= kPrime;
      hash ^= static_cast<uint64_t>(best1_idx);
      hash *= kPrime;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(best2_score));
      hash *= kPrime;
      hash ^= static_cast<uint64_t>(best2_idx);
      hash *= kPrime;
    }
  }
  return hash;
}
