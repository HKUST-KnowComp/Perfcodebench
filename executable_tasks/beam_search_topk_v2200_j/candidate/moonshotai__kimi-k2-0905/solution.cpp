#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

struct Better {
  __attribute__((always_inline))
  bool operator()(const std::pair<int32_t, int>& a,
                  const std::pair<int32_t, int>& b) const noexcept {
    return (a.first > b.first) || (a.first == b.first && a.second < b.second);
  }
};

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
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
  std::vector<std::pair<int32_t, int>> row;
  row.reserve(static_cast<size_t>(vocab));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      row.clear();
      const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        row.emplace_back(logits[base + static_cast<size_t>(v)], v);
      }

      // Select the (topk) smallest according to Better; then sort exactly.
      if (topk > 0) {
        std::nth_element(row.begin(), row.begin() + topk, row.end(), Better{});
        std::partial_sort(row.begin(), row.begin() + topk, row.end(), Better{});
      }

      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
        hash = mix(hash, static_cast<uint64_t>(row[k].second));
      }
    }
  }
  return hash;
}