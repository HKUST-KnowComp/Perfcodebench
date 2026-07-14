#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  std::vector<uint64_t> keys(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  const int32_t* logits_ptr = logits.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const int32_t* beam_logits = logits_ptr + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        uint32_t u_logit = static_cast<uint32_t>(beam_logits[v]) ^ 0x80000000U;
        keys[static_cast<std::size_t>(v)] = (static_cast<uint64_t>(u_logit) << 32) | static_cast<uint32_t>(~v);
      }
      
      std::partial_sort(keys.begin(), keys.begin() + topk, keys.end(), std::greater<uint64_t>());
      
      for (int k = 0; k < topk; ++k) {
        uint64_t key = keys[static_cast<std::size_t>(k)];
        uint32_t u_logit = static_cast<uint32_t>(key >> 32);
        uint32_t orig_logit = u_logit ^ 0x80000000U;
        uint32_t idx = ~static_cast<uint32_t>(key & 0xFFFFFFFFU);
        hash = mix(hash, static_cast<uint64_t>(orig_logit));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}
