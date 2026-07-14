#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

/**
 * FNV-1a style mix function as defined in the baseline.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (bins == 0) {
    return 0;
  }

  // Use a dense vector for the histogram to ensure O(1) access and cache friendliness.
  // Pre-allocating outside the iteration loop avoids repeated allocations.
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset the histogram counts efficiently.
    std::fill(counts.begin(), counts.end(), 0);

    // Accumulate counts. Since the task specifies a "dense histogram", 
    // we assume values[i] < bins.
    const uint32_t* val_ptr = values.data();
    size_t val_size = values.size();
    for (size_t i = 0; i < val_size; ++i) {
      uint32_t v = val_ptr[i];
      // Safety check is omitted for performance, assuming input validity per task context.
      counts[v]++;
    }

    // Calculate the checksum using the specific FNV-1a constants provided.
    hash = 1469598103934665603ULL;
    const uint32_t* count_ptr = counts.data();
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, static_cast<uint64_t>(count_ptr[b]));
    }
  }

  return hash;
}
