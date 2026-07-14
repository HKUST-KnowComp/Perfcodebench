#include "interface.h"

#include <algorithm>
#include <numeric>
#include <vector>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (iters <= 0) return 0;
  
  int k = std::min(topk, vocab);
  std::vector<int> idx(vocab);
  uint64_t hash = 1469598103934665603ULL;
  
  for (int b = 0; b < beams; ++b) {
    std::iota(idx.begin(), idx.end(), 0);
    const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
    
    auto comp = [beam_logits](int a, int b_idx) {
      int32_t va = beam_logits[a];
      int32_t vb = beam_logits[b_idx];
      return va > vb || (va == vb && a < b_idx);
    };
    
    if (k < vocab) {
      std::nth_element(idx.begin(), idx.begin() + k, idx.end(), comp);
      std::sort(idx.begin(), idx.begin() + k, comp);
    } else {
      std::sort(idx.begin(), idx.end(), comp);
    }
    
    for (int i = 0; i < k; ++i) {
      int token = idx[i];
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(beam_logits[token]));
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(token);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
