#include "interface.h"
#include <vector>

namespace {

// FNV-1a constants
const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
const uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= FNV_PRIME;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) return 0;

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint32_t segment_id = static_cast<uint32_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, segment_id);
      }
    }
    final_hash = hash;
  }
  return final_hash;
}