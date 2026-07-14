#include "interface.h"
#include <vector>
#include <cstring>
#include <cstdint>

namespace {

/**
 * FNV-1a mix function used in the original implementation.
 */
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= 1099511628211ULL;
  return hash;
}

const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  if (iters <= 0) return 0;

  // Use a vector as a dense array for the histogram.
  // Pre-allocating outside the loop to avoid repeated allocations.
  std::vector<uint32_t> counts(bins);
  uint32_t* const counts_ptr = counts.data();
  const size_t counts_bytes = bins * sizeof(uint32_t);

  const uint32_t* const val_ptr = values.data();
  const size_t val_size = values.size();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Efficiently clear the histogram for each iteration.
    std::memset(counts_ptr, 0, counts_bytes);

    // Accumulate counts. Only values < bins contribute to the final hash.
    for (size_t i = 0; i < val_size; ++i) {
      uint32_t v = val_ptr[i];
      if (v < bins) {
        counts_ptr[v]++;
      }
    }

    // Compute the FNV-1a checksum over the bin counts.
    hash = FNV_OFFSET_BASIS;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts_ptr[b]);
    }
  }

  return hash;
}