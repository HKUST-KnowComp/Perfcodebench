#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      // Reset indices for this beam
      for (int i = 0; i < vocab; ++i) {
        indices[static_cast<std::size_t>(i)] = i;
      }
      
      // Partial sort to get top-k elements
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(),
        [&logits, base](int a, int b) {
          int32_t val_a = logits[base + static_cast<std::size_t>(a)];
          int32_t val_b = logits[base + static_cast<std::size_t>(b)];
          return val_a > val_b || (val_a == val_b && a < b);
        });
      
      // Mix top-k results into hash
      for (int k = 0; k < topk; ++k) {
        int idx = indices[static_cast<std::size_t>(k)];
        int32_t score = logits[base + static_cast<std::size_t>(idx)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(score)));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}