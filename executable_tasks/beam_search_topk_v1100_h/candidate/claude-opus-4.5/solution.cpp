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
  const int k = topk;
  const int v_size = vocab;
  
  // Pre-allocate index array
  std::vector<int> indices(static_cast<std::size_t>(v_size));
  for (int i = 0; i < v_size; ++i) {
    indices[static_cast<std::size_t>(i)] = i;
  }
  
  // Working copy of indices for each beam
  std::vector<int> work(static_cast<std::size_t>(v_size));
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < beams; ++b) {
      const int32_t* row_logits = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(v_size);
      
      // Reset working indices
      std::copy(indices.begin(), indices.end(), work.begin());
      
      // Comparator: higher score is better, lower index breaks ties
      auto cmp = [row_logits](int a, int b) {
        const int32_t sa = row_logits[a];
        const int32_t sb = row_logits[b];
        return sa > sb || (sa == sb && a < b);
      };
      
      // Partition so that top-k elements are in work[0..k)
      // nth_element ensures work[k-1] is the k-th best element
      // and all elements before it are >= (by our comparator)
      if (k < v_size) {
        std::nth_element(work.begin(), work.begin() + k, work.end(), cmp);
      }
      
      // Sort only the top-k portion
      std::sort(work.begin(), work.begin() + k, cmp);
      
      // Compute hash for top-k
      for (int i = 0; i < k; ++i) {
        int idx = work[static_cast<std::size_t>(i)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_logits[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  
  return hash;
}