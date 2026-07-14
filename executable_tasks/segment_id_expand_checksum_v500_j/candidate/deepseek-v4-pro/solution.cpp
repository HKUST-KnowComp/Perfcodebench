#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0;
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = offsets.size();
  for (size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint32_t val = static_cast<uint32_t>(s);
    for (uint32_t i = 0; i < len; ++i) {
      hash = mix(hash, val);
    }
  }
  return hash;
}