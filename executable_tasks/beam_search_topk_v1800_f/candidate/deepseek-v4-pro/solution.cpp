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
  const std::size_t V = static_cast<std::size_t>(vocab);
  std::vector<std::pair<int32_t, int>> row(V);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * V;
      // Fill row
      for (std::size_t v = 0; v < V; ++v) {
        row[v].first = logits[base + v];
        row[v].second = static_cast<int>(v);
      }

      if (topk > 0) {
        if (static_cast<std::size_t>(topk) < V) {
          std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
          std::sort(row.begin(), row.begin() + topk, better);
        } else {
          std::sort(row.begin(), row.end(), better);
        }
        for (int k = 0; k < topk; ++k) {
          hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].first)));
          hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].second));
        }
      }
    }
  }
  return hash;
}