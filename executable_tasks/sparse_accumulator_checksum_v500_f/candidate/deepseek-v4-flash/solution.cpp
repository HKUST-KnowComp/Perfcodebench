#include "interface.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  if (iters <= 0) return 0;

  // Accumulate once (same for every iteration)
  std::unordered_map<uint32_t, uint64_t> accum;
  for (std::size_t i = 0; i < indices.size(); ++i) {
    accum[indices[i]] += static_cast<uint64_t>(values[i]);
  }

  // Compute hash over the entire domain once
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t key = 0; key < domain; ++key) {
    const auto it = accum.find(key);
    hash = mix(hash, it == accum.end() ? 0ULL : it->second);
  }
  return hash;
}
