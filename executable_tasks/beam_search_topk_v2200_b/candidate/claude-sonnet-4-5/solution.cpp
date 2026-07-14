#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

struct IndexedScore {
  int32_t score;
  int index;
  
  bool operator<(const IndexedScore& other) const {
    return score > other.score || (score == other.score && index < other.index);
  }
};

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<IndexedScore> row(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      // Prefetch next beam if available
      if (b + 1 < beams) {
        const std::size_t next_base = static_cast<std::size_t>(b + 1) * static_cast<std::size_t>(vocab);
        __builtin_prefetch(&logits[next_base], 0, 0);
      }
      
      // Build indexed scores
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<std::size_t>(v)] = {logits[base + static_cast<std::size_t>(v)], v};
      }
      
      // Partial sort: only sort the top-k elements
      std::partial_sort(row.begin(), 
                       row.begin() + topk, 
                       row.end());
      
      // Hash top-k results
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].score)));
        hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].index));
      }
    }
  }
  return hash;
}