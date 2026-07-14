#pragma once

#include <cstdint>
#include <vector>

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters);

#include <algorithm>
#include <utility>

namespace {

bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

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
      int32_t top1_logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts)];
      int top1_idx = 0;
      int32_t top2_logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + 1];
      int top2_idx = 1;

      if (better({top2_logit, top2_idx}, {top1_logit, top1_idx})) {
        std::swap(top1_logit, top2_logit);
        std::swap(top1_idx, top2_idx);
      }

      for (int e = 2; e < experts; ++e) {
        int32_t logit = logits[static_cast<std::size_t>(t) * static_cast<std::size_t>(experts) + static_cast<std::size_t>(e)];
        if (better({logit, e}, {top1_logit, top1_idx})) {
          top2_logit = top1_logit;
          top2_idx = top1_idx;
          top1_logit = logit;
          top1_idx = e;
        } else if (better({logit, e}, {top2_logit, top2_idx})) {
          top2_logit = logit;
          top2_idx = e;
        }
      }

      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_logit)));
      hash = mix(hash, static_cast<uint64_t>(top1_idx));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_logit)));
      hash = mix(hash, static_cast<uint64_t>(top2_idx));
    }
  }
  return hash;
}