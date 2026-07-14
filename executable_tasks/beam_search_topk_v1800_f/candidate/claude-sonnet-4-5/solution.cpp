#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
  return a.first > b.first || (a.first == b.first && a.second < b.second);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<std::pair<int32_t, int>> row;
  row.reserve(static_cast<std::size_t>(vocab));
  
  // Pre-populate row with indices once
  for (int v = 0; v < vocab; ++v) {
    row.push_back({0, v});
  }
  
  uint64_t hash = 0;
  const std::size_t topk_size = static_cast<std::size_t>(topk);
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      // Update logits in-place
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<std::size_t>(v)].first = logits[base + static_cast<std::size_t>(v)];
      }
      
      // Use nth_element to partition top-k, then sort only those k elements
      std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
      std::sort(row.begin(), row.begin() + topk, better);
      
      // Hash top-k results
      for (std::size_t k = 0; k < topk_size; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
        hash = mix(hash, static_cast<uint64_t>(row[k].second));
      }
    }
  }
  return hash;
}
