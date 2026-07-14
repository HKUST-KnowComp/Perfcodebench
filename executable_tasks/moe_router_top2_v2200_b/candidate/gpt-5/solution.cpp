#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  const int32_t* data = logits.data();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;  // FNV-1a offset basis
    for (int t = 0; t < tokens; ++t) {
      const int32_t* base = data + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);

      int b1i = -1, b2i = -1;
      int32_t b1v = 0, b2v = 0;

      for (int e = 0; e < experts; ++e) {
        int32_t v = base[e];
        if (b1i < 0) {
          b1i = e; b1v = v;
          continue;
        }
        if (b2i < 0) {
          // Ensure b1 is better than b2 according to comparator
          if (v > b1v || (v == b1v && e < b1i)) {
            b2i = b1i; b2v = b1v;
            b1i = e; b1v = v;
          } else {
            b2i = e; b2v = v;
          }
          continue;
        }
        // Now both b1 and b2 are set; maintain top-2 ordering
        if (v > b1v || (v == b1v && e < b1i)) {
          b2i = b1i; b2v = b1v;
          b1i = e; b1v = v;
        } else if (v > b2v || (v == b2v && e < b2i)) {
          b2i = e; b2v = v;
        }
      }

      // Mix results into hash (match baseline casting behavior)
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(b1v)));
      hash = mix(hash, static_cast<uint64_t>(b1i));
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(b2v)));
      hash = mix(hash, static_cast<uint64_t>(b2i));
    }
  }
  return hash;
}
