#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preallocate dense count array once to avoid repeated hash table allocation overhead
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Fast bulk reset of contiguous memory, far faster than unordered_map::clear()
    std::fill(counts.begin(), counts.end(), 0);

    // Accumulate values with direct array indexing, no hash computation overhead
    for (uint32_t v : values) {
      ++counts[v];
    }

    // Compute checksum with sequential, prefetch-friendly array accesses
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }

  return hash;
}