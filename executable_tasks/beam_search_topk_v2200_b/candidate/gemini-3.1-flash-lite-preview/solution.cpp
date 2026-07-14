#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<int> indices(static_cast<size_t>(vocab));
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* row_start = &logits[static_cast<size_t>(b) * vocab];
      
      std::iota(indices.begin(), indices.end(), 0);
      
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(), [&](int i, int j) {
        int32_t val_i = row_start[i];
        int32_t val_j = row_start[j];
        if (val_i != val_j) return val_i > val_j;
        return i < j;
      });

      for (int k = 0; k < topk; ++k) {
        int idx = indices[k];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row_start[idx])));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}