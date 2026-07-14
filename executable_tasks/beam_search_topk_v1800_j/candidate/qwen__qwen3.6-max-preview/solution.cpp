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
  std::vector<uint64_t> row(static_cast<std::size_t>(vocab));
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int b = 0; b < beams; ++b) {
      const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
      for (int v = 0; v < vocab; ++v) {
        uint32_t u_logit = static_cast<uint32_t>(logits[base + static_cast<std::size_t>(v)]) ^ 0x80000000U;
        uint32_t u_idx = ~static_cast<uint32_t>(v);
        row[static_cast<std::size_t>(v)] = (static_cast<uint64_t>(u_logit) << 32) | u_idx;
      }

      auto mid = row.begin() + topk;
      std::partial_sort(row.begin(), mid, row.end(), std::greater<uint64_t>());

      for (int k = 0; k < topk; ++k) {
        uint64_t packed = row[static_cast<std::size_t>(k)];
        uint32_t u_logit = static_cast<uint32_t>(packed >> 32);
        int32_t logit = static_cast<int32_t>(u_logit ^ 0x80000000U);
        int idx = static_cast<int>(~static_cast<uint32_t>(packed));

        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(logit)));
        hash = mix(hash, static_cast<uint64_t>(idx));
      }
    }
  }
  return hash;
}