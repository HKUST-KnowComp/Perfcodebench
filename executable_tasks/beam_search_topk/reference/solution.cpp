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
  std::vector<std::pair<int32_t, int>> best(static_cast<std::size_t>(topk));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      for (int k = 0; k < topk; ++k) {
        best[static_cast<std::size_t>(k)] = {-2147483647 - 1, vocab};
      }
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        const std::pair<int32_t, int> cur{logits[base + static_cast<std::size_t>(v)], v};
        if (!better(cur, best[static_cast<std::size_t>(topk - 1)])) {
          continue;
        }
        int pos = topk - 1;
        while (pos > 0 && better(cur, best[static_cast<std::size_t>(pos - 1)])) {
          best[static_cast<std::size_t>(pos)] = best[static_cast<std::size_t>(pos - 1)];
          --pos;
        }
        best[static_cast<std::size_t>(pos)] = cur;
      }
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(best[static_cast<std::size_t>(k)].first)));
        hash = mix(hash, static_cast<uint64_t>(best[static_cast<std::size_t>(k)].second));
      }
    }
  }
  return hash;
}
