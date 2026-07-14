#include "interface.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Insert (score,id) into a fixed-size top-k array (k<=32) in descending order.
// Keeps the smallest element at position k-1 for quick eviction.
inline void insert_topk(std::pair<int32_t, int>* topk, int k,
                        int32_t score, int id) {
  // Fast path: smaller than current k-th -> skip
  if (k > 0 && score < topk[k - 1].first) return;
  if (k > 0 && score == topk[k - 1].first && id > topk[k - 1].second) return;

  // Find position; shift right; insert
  int pos = 0;
  while (pos < k) {
    if (score > topk[pos].first ||
        (score == topk[pos].first && id < topk[pos].second))
      break;
    ++pos;
  }
  if (pos < k) {
    for (int j = k - 1; j > pos; --j) topk[j] = topk[j - 1];
    topk[pos] = {score, id};
  }
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams, int vocab, int topk, int iters) {
  // Clamp topk to a reasonable upper bound to keep the array small
  const int k = (topk > 32) ? 32 : topk;
  std::pair<int32_t, int> topk_buf[32];
  uint64_t hash = 14695981039346656037ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 14695981039346656037ULL;
    for (int b = 0; b < beams; ++b) {
      // Initialize buffer to minimal values
      for (int i = 0; i < k; ++i) topk_buf[i] = {INT32_MIN, 0};

      const int32_t* ptr = &logits[static_cast<size_t>(b) * vocab];
      for (int v = 0; v < vocab; ++v) {
        insert_topk(topk_buf, k, ptr[v], v);
      }

      // Mix the top-k results into the checksum
      for (int i = 0; i < k; ++i) {
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(topk_buf[i].first)));
        hash = mix(hash, static_cast<uint64_t>(topk_buf[i].second));
      }
    }
  }
  return hash;
}