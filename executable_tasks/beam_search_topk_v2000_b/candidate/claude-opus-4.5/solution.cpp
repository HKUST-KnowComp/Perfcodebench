#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
  int32_t score;
  int id;
};

inline bool better(const Entry& a, const Entry& b) {
  return a.score > b.score || (a.score == b.score && a.id < b.id);
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
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < beams; ++b) {
      const int32_t* base_ptr = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      for (int v = 0; v < vocab; ++v) {
        row[static_cast<std::size_t>(v)] = {base_ptr[v], v};
      }
      
      if (topk < vocab) {
        std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
        std::sort(row.begin(), row.begin() + topk, better);
      } else {
        std::sort(row.begin(), row.end(), better);
      }
      
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].score)));
        hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].id));
      }
    }
  }
  
  return hash;
}