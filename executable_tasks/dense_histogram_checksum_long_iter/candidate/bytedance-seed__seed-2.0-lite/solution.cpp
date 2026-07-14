#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preallocate dense bin storage once to avoid repeated allocations and hash overhead
  std::vector<uint32_t> counts(bins, 0);
  // Track only modified bins to minimize reset work between iterations
  std::vector<uint32_t> used_bins;
  used_bins.reserve(bins); // Preallocate maximum capacity to avoid reallocations

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    used_bins.clear();
    // Fast indexed count accumulation with no hash table lookups
    for (uint32_t v : values) {
      if (counts[v]++ == 0) {
        used_bins.push_back(v);
      }
    }

    // Exact same checksum calculation as baseline to preserve correctness
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }

    // Reset only used bins to zero, avoids full array zeroing overhead
    for (uint32_t v : used_bins) {
      counts[v] = 0;
    }
  }
  return hash;
}