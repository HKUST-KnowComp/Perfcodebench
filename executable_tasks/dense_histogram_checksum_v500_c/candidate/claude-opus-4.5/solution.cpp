#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  const uint32_t* __restrict vptr = values.data();
  const size_t n = values.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Clear histogram
    for (uint32_t b = 0; b < bins; ++b) {
      counts[b] = 0;
    }

    // Accumulate histogram with manual unrolling
    size_t i = 0;
    const size_t n4 = n & ~size_t(3);
    for (; i < n4; i += 4) {
      ++counts[vptr[i]];
      ++counts[vptr[i + 1]];
      ++counts[vptr[i + 2]];
      ++counts[vptr[i + 3]];
    }
    for (; i < n; ++i) {
      ++counts[vptr[i]];
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }

  return hash;
}
