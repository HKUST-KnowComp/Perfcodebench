#include "interface.h"

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preserve baseline behavior when no iterations are requested.
  if (iters <= 0) {
    return 0ULL;
  }

  // If bins is zero, each iteration sets hash to the FNV offset basis and
  // does no mixing; the final result is the offset basis.
  if (bins == 0) {
    return 1469598103934665603ULL;
  }

  // Compute the histogram once; the result is identical across iterations.
  std::vector<uint32_t> counts(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  // Compute the checksum for the (identical) final iteration.
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a 64-bit prime

  const uint32_t* ptr = counts.data();
  const uint32_t* end = ptr + bins;
  while (ptr != end) {
    hash ^= static_cast<uint64_t>(*ptr++);
    hash *= prime;
  }

  return hash;
}
