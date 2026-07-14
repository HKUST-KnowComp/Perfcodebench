#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
  int32_t score;
  int idx;
};

inline bool better(const Entry& a, const Entry& b) {
  return a.score > b.score || (a.score == b.score && a.idx < b.idx);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<Entry> row(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  const std::size_t k = static_cast<std::size_t>(topk);
  const std::size_t v_size = static_cast<std::size_t>(vocab);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* base_ptr = logits.data() + static_cast<std::size_t>(b) * v_size;
      for (std::size_t v = 0; v < v_size; ++v) {
        row[v].score = base_ptr[v];
        row[v].idx = static_cast<int>(v);
      }
      if (k < v_size) {
        std::nth_element(row.begin(), row.begin() + static_cast<std::ptrdiff_t>(k), row.end(), better);
      }
      std::sort(row.begin(), row.begin() + static_cast<std::ptrdiff_t>(k), better);
      for (std::size_t i = 0; i < k; ++i) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[i].score)));
        hash = mix(hash, static_cast<uint64_t>(row[i].idx));
      }
    }
  }
  return hash;
}