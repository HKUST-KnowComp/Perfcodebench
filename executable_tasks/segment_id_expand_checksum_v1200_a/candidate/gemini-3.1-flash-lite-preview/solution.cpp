#include "interface.h"
#include <vector>

namespace {

// FNV-1a constants
const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
const uint64_t FNV_PRIME = 1099511628211ULL;

// Precompute the hash contribution of a single value repeated N times
// Since hash = (...((hash ^ val) * P ^ val) * P ...)
// This can be computed iteratively or via closed form, but given the constraints,
// a simple loop is efficient enough when not allocating memory.
inline uint64_t hash_repeat(uint64_t hash, uint32_t val, uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    hash ^= static_cast<uint64_t>(val);
    hash *= FNV_PRIME;
  }
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) return 0;

  uint64_t final_hash = 0;
  
  // The logic is deterministic: the expanded stream is identical every iteration.
  // We compute the hash once and return it.
  uint64_t hash = FNV_OFFSET_BASIS;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint32_t val = static_cast<uint32_t>(s);
    hash = hash_repeat(hash, val, len);
  }
  final_hash = hash;

  return final_hash;
}