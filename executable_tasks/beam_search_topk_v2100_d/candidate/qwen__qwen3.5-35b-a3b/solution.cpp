#include "interface.h"

#include <algorithm>
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
  std::vector<int> indices(vocab);
  std::iota(indices.begin(), indices.end(), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      std::iota(indices.begin(), indices.end(), 0);
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(),
        [&](int i, int j) {
          int32_t score_i = logits[base + i];
          int32_t score_j = logits[base + j];
          if (score_i != score_j) {
            return score_i > score_j;
          }
          return i < j;
        });
      for (int k = 0; k < topk; ++k) {
        int idx = indices[k];
        int32_t score = logits[base + idx];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(score)));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}