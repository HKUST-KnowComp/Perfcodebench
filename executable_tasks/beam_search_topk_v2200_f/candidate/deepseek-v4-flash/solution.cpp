#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace {
  // Comparator: descending logit, ascending index.
  bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
    return a.first > b.first || (a.first == b.first && a.second < b.second);
  }

  // FNV-1a 64-bit mix.
  uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
  }
}  // anonymous namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
  uint64_t hash;
  // Pre-allocate the working vector for one beam.
  std::vector<std::pair<int32_t, int>> row(static_cast<std::size_t>(vocab));

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);

      // Fill row directly by assignment (avoids push_back overhead).
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<std::size_t>(v)] = {logits[base + static_cast<std::size_t>(v)], v};
      }

      // Only the top-k elements need to be in sorted order.
      std::partial_sort(row.begin(), row.begin() + topk, row.end(), better);

      // Mix the top-k (logit, index) pairs into the hash.
      for (int k = 0; k < topk; ++k) {
        const auto& p = row[static_cast<std::size_t>(k)];
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(p.first)));
        hash = mix(hash, static_cast<uint64_t>(p.second));
      }
    }
  }

  return hash;
}
