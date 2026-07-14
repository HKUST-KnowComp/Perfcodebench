#include "interface.h"

#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  // Detect whether keys are exactly 0, 1, 2, ..., n-1
  const bool dense = !keys.empty() &&
                     keys[0] == 0 &&
                     keys.back() == static_cast<uint32_t>(keys.size() - 1);

  // Precompute the value index for every probe
  std::vector<uint32_t> idx(probes.size());
  if (dense) {
    for (size_t i = 0; i < probes.size(); ++i) {
      idx[i] = probes[i];
    }
  } else {
    for (size_t i = 0; i < probes.size(); ++i) {
      auto it = std::lower_bound(keys.begin(), keys.end(), probes[i]);
      idx[i] = static_cast<uint32_t>(it - keys.begin());
    }
  }

  const uint32_t* __restrict idx_ptr = idx.data();
  const uint32_t* __restrict val_ptr = values.data();
  const size_t n = probes.size();
  const uint64_t kInitialHash = 1469598103934665603ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = kInitialHash;
    for (size_t i = 0; i < n; ++i) {
      hash = mix(hash, val_ptr[idx_ptr[i]]);
    }
  }

  return hash;
}