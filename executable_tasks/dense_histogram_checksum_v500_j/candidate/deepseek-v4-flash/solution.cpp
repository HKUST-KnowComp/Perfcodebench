#include "interface.h"
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  const uint32_t* val_ptr = values.data();
  const size_t num_vals = values.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts to zero
    if (bins > 0) {
      std::memset(counts.data(), 0, bins * sizeof(uint32_t));
    }
    // Accumulate histogram (only for valid bin indices)
    for (size_t i = 0; i < num_vals; ++i) {
      const uint32_t v = val_ptr[i];
      if (v < bins) {
        ++counts[v];
      }
    }
    // Compute FNV-1a-like hash over bins
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}
