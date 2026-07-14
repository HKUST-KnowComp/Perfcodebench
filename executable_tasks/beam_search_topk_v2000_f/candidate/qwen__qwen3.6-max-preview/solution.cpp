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
  std::vector<uint64_t> packed(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;
  int limit = (topk < vocab) ? topk : vocab;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      const int32_t* logits_ptr = logits.data() + base;
      for (int v = 0; v < vocab; ++v) {
        uint32_t u_logit = static_cast<uint32_t>(logits_ptr[v]) ^ 0x80000000u;
        packed[static_cast<std::size_t>(v)] = (static_cast<uint64_t>(u_logit) << 32) | (~static_cast<uint32_t>(v) & 0xFFFFFFFFu);
      }

      if (limit < vocab) {
        std::nth_element(packed.begin(), packed.begin() + limit, packed.end(), std::greater<uint64_t>());
      }
      std::sort(packed.begin(), packed.begin() + limit, std::greater<uint64_t>());

      for (int k = 0; k < limit; ++k) {
        uint64_t p = packed[static_cast<std::size_t>(k)];
        uint32_t u_logit = static_cast<uint32_t>(p >> 32);
        uint32_t idx = ~static_cast<uint32_t>(p);
        hash = mix(hash, static_cast<uint64_t>(u_logit ^ 0x80000000u));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}