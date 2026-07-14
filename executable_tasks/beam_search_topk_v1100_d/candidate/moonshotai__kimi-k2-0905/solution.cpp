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

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
  std::vector<int> top_idx;
  top_idx.reserve(static_cast<size_t>(topk));
  std::vector<int32_t> top_val;
  top_val.reserve(static_cast<size_t>(topk));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);

      top_idx.clear();
      top_val.clear();

      for (int v = 0; v < vocab; ++v) {
        int32_t val = logits[base + static_cast<size_t>(v)];
        int pos = 0;
        while (pos < static_cast<int>(top_val.size()) &&
               (top_val[pos] > val ||
                (top_val[pos] == val && top_idx[pos] < v))) {
          ++pos;
        }

        if (pos < topk) {
          if (static_cast<int>(top_val.size()) < topk) {
            top_val.push_back(0);
            top_idx.push_back(0);
          }
          for (int i = static_cast<int>(top_val.size()) - 1; i > pos; --i) {
            top_val[i] = top_val[i - 1];
            top_idx[i] = top_idx[i - 1];
          }
          top_val[pos] = val;
          top_idx[pos] = v;
        }
      }

      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top_val[k])));
        hash = mix(hash, static_cast<uint64_t>(top_idx[k]));
      }
    }
  }
  return hash;
}