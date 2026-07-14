#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t final_hash = 0;
  const uint32_t n_seg = static_cast<uint32_t>(offsets.size() - 1);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t s = 0; s < n_seg; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, static_cast<uint64_t>(s));
      }
    }
    final_hash = hash;
  }
  return final_hash;
}