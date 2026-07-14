#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

struct Entry {
  int32_t logit;
  int id;
};

inline bool better(const Entry& a, const Entry& b) {
  return a.logit > b.logit || (a.logit == b.logit && a.id < b.id);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<Entry> row(vocab);
  
  // Pre-initialize ids once
  for (int v = 0; v < vocab; ++v) {
    row[v].id = v;
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      
      // Copy logits only (ids already set)
      for (int v = 0; v < vocab; ++v) {
        row[v].logit = logits[base + v];
      }
      
      // Use nth_element to partition around topk, then sort only the top-k
      if (topk < vocab) {
        std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
      }
      
      // Sort only the top-k elements
      std::sort(row.begin(), row.begin() + topk, better);
      
      // Hash the top-k
      for (int k = 0; k < topk; ++k) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].logit)));
        hash = mix(hash, static_cast<uint64_t>(row[k].id));
      }
    }
  }
  
  return hash;
}
