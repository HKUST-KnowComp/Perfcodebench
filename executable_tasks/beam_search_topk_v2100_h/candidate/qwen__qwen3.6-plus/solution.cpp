#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<std::pair<int32_t, int32_t>> row(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  const std::size_t k_sz = static_cast<std::size_t>(topk);
  const std::size_t v_sz = static_cast<std::size_t>(vocab);
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* base = logits.data() + static_cast<std::size_t>(b) * v_sz;
      for (std::size_t v = 0; v < v_sz; ++v) {
        row[v] = {base[v], -static_cast<int32_t>(v)};
      }
      
      std::partial_sort(row.begin(), row.begin() + k_sz, row.end(),
                        std::greater<std::pair<int32_t, int32_t>>());
                        
      for (std::size_t k = 0; k < k_sz; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(-row[k].second)));
      }
    }
  }
  return hash;
}
