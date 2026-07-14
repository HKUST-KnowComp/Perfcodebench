#include "interface.h"

#include <algorithm>
#include <queue>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<int32_t> values(static_cast<std::size_t>(vocab));
  std::vector<int> indices(static_cast<std::size_t>(vocab));
  for (int i = 0; i < vocab; ++i) {
    indices[static_cast<std::size_t>(i)] = i;
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        values[static_cast<std::size_t>(v)] = logits[base + static_cast<std::size_t>(v)];
      }
      std::partial_sort(indices.begin(), indices.begin() + topk, indices.end(),
                        [&values](int a, int b) {
                          return values[static_cast<std::size_t>(a)] > values[static_cast<std::size_t>(b)] ||
                                 (values[static_cast<std::size_t>(a)] == values[static_cast<std::size_t>(b)] && a < b);
                        });
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(values[static_cast<std::size_t>(indices[static_cast<std::size_t>(k)])])));
        hash = mix(hash, static_cast<uint64_t>(indices[static_cast<std::size_t>(k)]));
      }
    }
  }
  return hash;
}
