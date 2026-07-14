#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = offsets.size();
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint32_t sid = static_cast<uint32_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, sid);
      }
    }
  }
  return hash;
}