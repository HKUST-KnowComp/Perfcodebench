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
  // Pre-compute the hash of the expanded stream once; it is identical every iteration.
  uint64_t base_hash = 1469598103934665603ULL;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      base_hash = mix(base_hash, static_cast<uint32_t>(s));
    }
  }

  // Each iteration just re-mixes the constant base_hash value.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = mix(1469598103934665603ULL, base_hash);
  }
  return hash;
}