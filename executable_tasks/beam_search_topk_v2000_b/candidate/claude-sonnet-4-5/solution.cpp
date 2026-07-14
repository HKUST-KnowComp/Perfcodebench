#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct IndexComparator {
  const int32_t* logits;
  
  bool operator()(int a, int b) const {
    int32_t val_a = logits[a];
    int32_t val_b = logits[b];
    return val_a > val_b || (val_a == val_b && a < b);
  }
};

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<int> indices(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      const int32_t* beam_logits = logits.data() + base;
      
      for (int v = 0; v < vocab; ++v) {
        indices[static_cast<std::size_t>(v)] = v;
      }
      
      IndexComparator comp{beam_logits};
      std::partial_sort(indices.begin(), 
                       indices.begin() + topk, 
                       indices.end(), 
                       comp);
      
      for (int k = 0; k < topk; ++k) {
        int idx = indices[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(beam_logits[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  
  return hash;
}