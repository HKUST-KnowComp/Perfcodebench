#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {
inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  const size_t vocab_sz = static_cast<size_t>(vocab);
  const size_t topk_sz = static_cast<size_t>(topk);
  std::vector<std::pair<int32_t, int>> row(vocab_sz);
  const int32_t* logits_ptr = logits.data();
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const size_t base = static_cast<size_t>(b) * vocab_sz;
      for (size_t v = 0; v < vocab_sz; ++v) {
        row[v] = {logits_ptr[base + v], static_cast<int>(v)};
      }
      std::partial_sort(row.begin(), row.begin() + topk_sz, row.end(), better);
      for (size_t k = 0; k < topk_sz; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
        hash = mix(hash, static_cast<uint64_t>(row[k].second));
      }
    }
    final_hash = hash;
  }
  return final_hash;
}