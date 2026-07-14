#include "interface.h"

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  (void)iters;
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      hash = mix(hash, static_cast<uint32_t>(s));
    }
  }
  return hash;
}