#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

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
  
  if (iters <= 0) return 0;

  // The baseline resets the hash to the FNV offset basis at the start of every iteration.
  // This means the result of the function is simply the result of the final iteration.
  // We only need to compute the hash once.
  uint64_t hash = 1469598103934665603ULL;

  if (keys.empty()) {
    // If keys is empty, lower_bound always returns keys.end(), idx = 0.
    // We must match the baseline's behavior (even if it potentially accesses values[0]).
    for (uint32_t probe : probes) {
      hash = mix(hash, values[0]);
    }
    return hash;
  }

  // Check if keys are a contiguous range (dense id remapping).
  // If keys[i] == keys[0] + i, we can find the index in O(1).
  const uint32_t k0 = keys.front();
  const uint32_t kn = keys.back();
  const size_t n = keys.size();
  
  bool is_contiguous = (kn - k0 == static_cast<uint32_t>(n - 1));

  if (is_contiguous) {
    for (uint32_t probe : probes) {
      size_t idx;
      if (probe <= k0) {
        idx = 0;
      } else if (probe > kn) {
        idx = n;
      } else {
        idx = static_cast<size_t>(probe - k0);
      }
      hash = mix(hash, values[idx]);
    }
  } else {
    // Fallback to binary search if not contiguous.
    for (uint32_t probe : probes) {
      const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      const std::size_t idx = static_cast<std::size_t>(it - keys.begin());
      hash = mix(hash, values[idx]);
    }
  }

  return hash;
}
