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
  for (int v = 0; v < vocab; ++v) {
    indices[static_cast<std::size_t>(v)] = v;
  }
  
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* beam_logits = logits_ptr + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(),
        [beam_logits](int a, int b) {
          int32_t val_a = beam_logits[a];
          int32_t val_b = beam_logits[b];
          return val_a > val_b || (val_a == val_b && a < b);
        });
      
      for (int k = 0; k < topk; ++k) {
        int idx = indices[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(beam_logits[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
      
      for (int v = 0; v < vocab; ++v) {
        indices[static_cast<std::size_t>(v)] = v;
      }
    }
  }
  return hash;
}