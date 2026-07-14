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
  // Use a dense array instead of a map for O(1) access and better cache locality
  std::vector<uint32_t> counts(bins, 0);
  uint32_t* counts_ptr = counts.data();
  const uint32_t* values_ptr = values.data();
  const size_t values_size = values.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for this iteration
    std::memset(counts_ptr, 0, bins * sizeof(uint32_t));

    // Accumulate histogram
    for (size_t i = 0; i < values_size; ++i) {
      uint32_t v = values_ptr[i];
      if (v < bins) {
        counts_ptr[v]++;
      }
    }

    // Calculate checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts_ptr[b]);
    }
  }
  return hash;
}