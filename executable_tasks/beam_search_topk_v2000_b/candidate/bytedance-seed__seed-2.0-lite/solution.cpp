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
  // Preinitialize row with fixed vocab IDs, only update logits per beam
  std::vector<std::pair<int32_t, int>> row(static_cast<std::size_t>(vocab));
  for (int v = 0; v < vocab; ++v) {
    row[v].second = v;
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      // Update only logit values, IDs are pre-set
      for (int v = 0; v < vocab; ++v) {
        row[v].first = logits[base + static_cast<std::size_t>(v)];
      }
      // Partial sort only the top-k elements, maintains exact same order as full sort
      std::partial_sort(row.begin(), row.begin() + topk, row.end(), better);
      for (int k = 0; k < topk; ++k) {
        const auto& elem = row[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(elem.first)));
        hash = mix(hash, static_cast<uint64_t>(elem.second));
      }
    }
  }
  return hash;
}