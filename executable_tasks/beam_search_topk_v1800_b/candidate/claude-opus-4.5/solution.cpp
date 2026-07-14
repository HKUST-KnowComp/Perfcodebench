#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  for (int i = 0; i < vocab; ++i) {
    indices[static_cast<std::size_t>(i)] = i;
  }
  
  std::vector<int> work(static_cast<std::size_t>(vocab));
  
  uint64_t hash = 0;
  const std::size_t k = static_cast<std::size_t>(topk);
  const std::size_t v_size = static_cast<std::size_t>(vocab);
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* row = logits.data() + static_cast<std::size_t>(b) * v_size;
      
      // Copy indices for this beam
      std::copy(indices.begin(), indices.end(), work.begin());
      
      // Comparator: higher score wins, ties broken by lower index
      auto cmp = [row](int a, int b) {
        int32_t sa = row[a];
        int32_t sb = row[b];
        return sa > sb || (sa == sb && a < b);
      };
      
      // Partial sort to get top-k elements
      std::partial_sort(work.begin(), work.begin() + static_cast<std::ptrdiff_t>(k), work.end(), cmp);
      
      // Hash the top-k results
      for (std::size_t i = 0; i < k; ++i) {
        int idx = work[i];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}
