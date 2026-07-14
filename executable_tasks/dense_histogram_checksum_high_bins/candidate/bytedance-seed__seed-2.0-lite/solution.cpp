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
  // Preallocate dense contiguous count array once, avoids unordered_map hash overhead
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate counts with direct, cache-friendly memory access
    for (uint32_t v : values) {
      ++counts[v];
    }

    // Compute exact matching checksum per original specification
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }

    // Efficiently reset only used bins by reversing increments (avoids zeroing large sparse arrays for high bins)
    for (uint32_t v : values) {
      --counts[v];
    }
  }

  return hash;
}