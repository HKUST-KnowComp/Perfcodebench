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
  // Use a dense vector instead of unordered_map for O(1) access and cache efficiency.
  // Since bins can be high, we allocate once.
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for the next iteration
    std::fill(counts.begin(), counts.end(), 0);

    // Accumulate counts
    for (uint32_t v : values) {
      // The problem implies bins is the bound for values we care about in the checksum.
      // However, the checksum loop only iterates up to 'bins'. 
      // If values can exceed 'bins', they don't affect the checksum.
      if (v < bins) {
        ++counts[v];
      }
    }

    // Calculate checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, static_cast<uint64_t>(counts[b]));
    }
  }

  return hash;
}
