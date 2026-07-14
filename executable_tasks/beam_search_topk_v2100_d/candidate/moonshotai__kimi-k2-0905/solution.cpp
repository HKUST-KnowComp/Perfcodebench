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

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
  std::vector<std::pair<int32_t, int>> row(static_cast<size_t>(vocab));
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    if (iter != 0) hash = 1469598103934665603ULL;

    for (int b = 0; b < beams; ++b) {
      const int32_t* base_ptr = &logits[static_cast<size_t>(b) * static_cast<size_t>(vocab)];

      // fill
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<size_t>(v)].first  = base_ptr[v];
        row[static_cast<size_t>(v)].second = v;
      }

      // nth_element to partition top-k
      std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
      // sort only the top-k slice for exact order
      std::sort(row.begin(), row.begin() + topk, better);

      // unrolled mixing
      const auto* top = row.data();
      int k = 0;
      for (; k + 3 < topk; k += 4) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
        hash = mix(hash, static_cast<uint64_t>(top[k].second));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+1].first)));
        hash = mix(hash, static_cast<uint64_t>(top[k+1].second));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+2].first)));
        hash = mix(hash, static_cast<uint64_t>(top[k+2].second));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k+3].first)));
        hash = mix(hash, static_cast<uint64_t>(top[k+3].second));
      }
      for (; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
        hash = mix(hash, static_cast<uint64_t>(top[k].second));
      }
    }
  }
  return hash;
}