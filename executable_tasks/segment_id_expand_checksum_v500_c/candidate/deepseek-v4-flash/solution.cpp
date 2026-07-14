#include "interface.h"

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
  std::vector<uint32_t> lengths(num_segments);
  for (size_t s = 0; s < num_segments; ++s) {
    lengths[s] = offsets[s + 1] - offsets[s];
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t s = 0; s < num_segments; ++s) {
      const uint32_t len = lengths[s];
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, static_cast<uint32_t>(s));
      }
    }
  }
  return hash;
}
