#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<int> idx(static_cast<std::size_t>(vocab));
  uint64_t hash;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      // fill index vector with 0..vocab-1
      std::iota(idx.begin(), idx.end(), 0);
      // bring top-k elements to the front in sorted order (descending logit, ascending index)
      std::partial_sort(idx.begin(), idx.begin() + topk, idx.end(),
          [&logits, base](int a, int b) {
            int32_t la = logits[base + static_cast<std::size_t>(a)];
            int32_t lb = logits[base + static_cast<std::size_t>(b)];
            if (la != lb) return la > lb;
            return a < b;
          });
      // hash the top-k tokens
      for (int k = 0; k < topk; ++k) {
        std::size_t token_idx = static_cast<std::size_t>(idx[static_cast<std::size_t>(k)]);
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(logits[base + token_idx])));
        hash = mix(hash, static_cast<uint64_t>(token_idx));
      }
    }
  }
  return hash;
}
