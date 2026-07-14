#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

struct IndexedValue {
  int32_t value;
  int index;
};

inline bool better(const IndexedValue& a, const IndexedValue& b) {
  return a.value > b.value || (a.value == b.value && a.index < b.index);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<IndexedValue> row(vocab);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      for (int v = 0; v < vocab; ++v) {
        row[v].value = logits[base + v];
        row[v].index = v;
      }
      
      std::partial_sort(row.begin(), row.begin() + topk, row.end(), better);
      
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].value)));
        hash = mix(hash, static_cast<uint64_t>(row[k].index));
      }
    }
  }
  return hash;
}