#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
      const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
      std::iota(indices.begin(), indices.end(), 0);
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(),
          [&logits, base](int a, int b) {
            int32_t sa = logits[base + a];
            int32_t sb = logits[base + b];
            return sa > sb || (sa == sb && a < b);
          });
      for (int k = 0; k < topk; ++k) {
        int idx = indices[k];
        int32_t score = logits[base + static_cast<size_t>(idx)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(score)));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}
