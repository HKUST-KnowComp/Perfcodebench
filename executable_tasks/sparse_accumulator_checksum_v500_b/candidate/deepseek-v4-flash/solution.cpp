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

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Dense accumulator and epoch (version) array
  std::vector<uint64_t> acc(domain);
  std::vector<uint32_t> epoch(domain, 0);
  uint64_t current_epoch = 0;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    ++current_epoch;
    // Accumulate updates with lazy reset using epoch
    for (std::size_t i = 0; i < indices.size(); ++i) {
      uint32_t idx = indices[i];
      if (epoch[idx] != current_epoch) {
        epoch[idx] = current_epoch;
        acc[idx] = 0;
      }
      acc[idx] += static_cast<uint64_t>(values[i]);
    }
    // Compute hash over entire domain
    hash = 1469598103934665603ULL;
    for (uint32_t key = 0; key < domain; ++key) {
      uint64_t val = (epoch[key] == current_epoch) ? acc[key] : 0ULL;
      hash = mix(hash, val);
    }
  }
  return hash;
}
