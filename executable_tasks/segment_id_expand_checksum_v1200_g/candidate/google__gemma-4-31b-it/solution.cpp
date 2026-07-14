#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t final_hash = 0;
  const size_t num_offsets = offsets.size();
  
  if (num_offsets < 2) {
    // If there are fewer than 2 offsets, the expanded stream is empty.
    // The baseline logic would set hash = 1469598103934665603ULL and loop 0 times.
    if (iters > 0) {
        final_hash = 1469598103934665603ULL;
    }
    return final_hash;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < num_offsets; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t val = static_cast<uint64_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, val);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}