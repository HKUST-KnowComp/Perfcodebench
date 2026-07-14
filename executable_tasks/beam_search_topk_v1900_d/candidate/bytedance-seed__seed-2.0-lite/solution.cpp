#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

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

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  // Preallocate working array and initialize fixed vocab indices once
  const size_t vocab_size = static_cast<size_t>(vocab);
  const size_t topk_size = static_cast<size_t>(topk);
  std::vector<std::pair<int32_t, int>> row(vocab_size);
  for (size_t v = 0; v < vocab_size; ++v) {
    row[v].second = static_cast<int>(v);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const size_t base = static_cast<size_t>(b) * vocab_size;
      // Only update logit values, indices are already preinitialized
      for (size_t v = 0; v < vocab_size; ++v) {
        row[v].first = logits[base + v];
      }
      // Partial sort to only order the top-k elements, far faster than full sort
      std::partial_sort(row.begin(), row.begin() + topk_size, row.end(), better);
      // Generate identical checksum to baseline implementation
      for (size_t k = 0; k < topk_size; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
        hash = mix(hash, static_cast<uint64_t>(row[k].second));
      }
    }
  }
  return hash;
}