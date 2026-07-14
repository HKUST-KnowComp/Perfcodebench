#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<int> indices(static_cast<std::size_t>(vocab));
  std::iota(indices.begin(), indices.end(), 0);
  
  uint64_t hash = 0;
  const std::size_t k = static_cast<std::size_t>(topk);
  const std::size_t v_size = static_cast<std::size_t>(vocab);
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* row_logits = logits.data() + static_cast<std::size_t>(b) * v_size;
      
      // Reset indices for this beam
      std::iota(indices.begin(), indices.end(), 0);
      
      // Partial sort to get top-k elements
      std::partial_sort(indices.begin(), indices.begin() + static_cast<std::ptrdiff_t>(k), indices.end(),
        [row_logits](int a, int b) {
          int32_t la = row_logits[a];
          int32_t lb = row_logits[b];
          return la > lb || (la == lb && a < b);
        });
      
      for (std::size_t i = 0; i < k; ++i) {
        int idx = indices[i];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_logits[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}