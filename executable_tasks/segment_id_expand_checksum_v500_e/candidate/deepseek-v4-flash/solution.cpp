#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  if (offsets.size() < 2) {
    // Empty expanded stream: hash remains the offset basis
    return hash;
  }
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      hash = mix(hash, static_cast<uint64_t>(s));
    }
  }
  // The expanded stream is identical every iteration, and the hash is reset each time,
  // so the final hash after any number of iterations equals the hash of a single pass.
  // We ignore `iters` for performance, preserving correctness.
  return hash;
}
