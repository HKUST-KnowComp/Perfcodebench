#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Use a dense vector for counts to avoid unordered_map overhead
  // Pre-allocate to avoid repeated allocations
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  const uint32_t* val_ptr = values.data();
  size_t val_size = values.size();
  uint32_t* count_ptr = counts.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Fast clear of the counts array
    std::memset(count_ptr, 0, bins * sizeof(uint32_t));

    // Accumulate histogram
    for (size_t i = 0; i < val_size; ++i) {
      uint32_t v = val_ptr[i];
      if (v < bins) {
        count_ptr[v]++;
      }
    }

    // Calculate checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, count_ptr[b]);
    }
  }

  return hash;
}