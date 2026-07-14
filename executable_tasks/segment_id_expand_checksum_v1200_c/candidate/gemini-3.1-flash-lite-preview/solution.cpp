#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) return 0;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t segment_id = static_cast<uint64_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, segment_id);
      }
    }
  }
  return hash;
}