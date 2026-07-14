#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>
#include <functional>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (iters <= 0) return 0;
  
  std::vector<uint64_t> row(static_cast<std::size_t>(vocab));
  uint64_t hash = 1469598103934665603ULL;
  
  const int k_limit = (topk < vocab) ? topk : vocab;
  
  for (int b = 0; b < beams; ++b) {
    const int32_t* beam_logits = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
    for (int v = 0; v < vocab; ++v) {
      uint32_t u_logit = static_cast<uint32_t>(beam_logits[v]) ^ 0x80000000u;
      row[static_cast<std::size_t>(v)] = (static_cast<uint64_t>(u_logit) << 32) | static_cast<uint32_t>(~static_cast<uint32_t>(v));
    }
    
    auto mid = row.begin() + k_limit;
    std::nth_element(row.begin(), mid, row.end(), std::greater<uint64_t>());
    std::sort(row.begin(), mid, std::greater<uint64_t>());
    
    for (int k = 0; k < k_limit; ++k) {
      uint64_t p = row[static_cast<std::size_t>(k)];
      uint32_t u_logit = static_cast<uint32_t>(p >> 32);
      int32_t logit = static_cast<int32_t>(u_logit ^ 0x80000000u);
      int idx = static_cast<int>(~static_cast<uint32_t>(p));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(logit)));
      hash = mix(hash, static_cast<uint64_t>(idx));
    }
  }
  return hash;
}
