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
  const std::size_t vocab_size = static_cast<std::size_t>(vocab);
  const std::size_t topk_size = static_cast<std::size_t>(topk);
  std::vector<std::pair<int32_t, int>> row(vocab_size);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * vocab_size;
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<std::size_t>(v)] = {logits[base + static_cast<std::size_t>(v)], v};
      }
      std::partial_sort(row.begin(), row.begin() + topk_size, row.end(), better);
      for (int k = 0; k < topk; ++k) {
        const std::size_t k_idx = static_cast<std::size_t>(k);
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k_idx].first)));
        hash = mix(hash, static_cast<uint64_t>(row[k_idx].second));
      }
    }
  }
  return hash;
}