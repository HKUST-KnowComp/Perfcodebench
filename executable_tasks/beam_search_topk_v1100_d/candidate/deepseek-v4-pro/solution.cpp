#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  const int k = std::min(topk, vocab);
  std::vector<std::vector<std::pair<int32_t, int>>> topk_selections(beams);
  std::vector<std::pair<int32_t, int>> row;
  row.reserve(vocab);
  for (int b = 0; b < beams; ++b) {
    row.clear();
    const int32_t* beam_data = logits.data() + b * vocab;
    for (int v = 0; v < vocab; ++v) {
      row.push_back({beam_data[v], v});
    }
    if (k > 0) {
      auto mid = row.begin() + k;
      std::nth_element(row.begin(), mid, row.end(), better);
      std::sort(row.begin(), mid, better);
    }
    topk_selections[b].assign(row.begin(), row.begin() + k);
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const auto& sel = topk_selections[b];
      for (int kk = 0; kk < k; ++kk) {
        const auto& p = sel[kk];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(p.first)));
        hash = mix(hash, static_cast<uint64_t>(p.second));
      }
    }
  }
  return hash;
}
