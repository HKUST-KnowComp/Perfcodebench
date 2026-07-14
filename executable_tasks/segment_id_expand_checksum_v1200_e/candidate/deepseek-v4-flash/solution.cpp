#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t hash = 0;
  if (iters > 0) {
    const uint64_t init = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    hash = init;
    const std::size_t n = offsets.size();
    const uint32_t* data = offsets.data();
    for (std::size_t s = 0; s + 1 < n; ++s) {
      uint32_t len = data[s+1] - data[s];
      uint32_t val = static_cast<uint32_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash ^= val;
        hash *= prime;
      }
    }
  }
  return hash;
}