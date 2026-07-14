#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct IndexedValue {
  int32_t value;
  int index;
  
  inline bool operator<(const IndexedValue& other) const {
    return value > other.value || (value == other.value && index < other.index);
  }
};

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
      const int32_t* base_ptr = logits.data() + static_cast<size_t>(b) * static_cast<size_t>(vocab);
      
      for (int v = 0; v < vocab; ++v) {
        row[v].value = base_ptr[v];
        row[v].index = v;
      }
      
      std::partial_sort(row.begin(), row.begin() + topk, row.end());
      
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].value)));
        hash = mix(hash, static_cast<uint64_t>(row[k].index));
      }
    }
  }
  
  return hash;
}
