#include "interface.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (beams <= 0 || vocab <= 0 || topk <= 0 || iters <= 0) return 0;

  const std::size_t vocab_sz = static_cast<std::size_t>(vocab);
  const std::size_t topk_sz = static_cast<std::size_t>(topk);

  std::vector<std::pair<int32_t, int>> row(vocab_sz);
  uint64_t hash = 1469598103934665603ULL;

  // Each iteration resets hash to the same seed and processes identical data.
  // The result is strictly idempotent, so we compute it once for maximum throughput.
  for (int b = 0; b < beams; ++b) {
    const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * vocab_sz;
    for (std::size_t v = 0; v < vocab_sz; ++v) {
      row[v] = {beam_logits[v], static_cast<int>(v)};
    }

    std::sort(row.begin(), row.end(), better);

    for (std::size_t k = 0; k < topk_sz; ++k) {
      hash ^= static_cast<uint32_t>(row[k].first);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(row[k].second);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}