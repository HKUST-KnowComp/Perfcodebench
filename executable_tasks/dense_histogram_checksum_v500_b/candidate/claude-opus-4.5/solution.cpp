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
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;
  const size_t n = values.size();
  const uint32_t* __restrict vptr = values.data();
  uint32_t* __restrict cptr = counts.data();

  for (int iter = 0; iter < iters; ++iter) {
    // Clear histogram
    for (uint32_t b = 0; b < bins; ++b) {
      cptr[b] = 0;
    }

    // Accumulate histogram with manual unrolling
    size_t i = 0;
    const size_t unroll_limit = n & ~size_t(3);
    for (; i < unroll_limit; i += 4) {
      ++cptr[vptr[i]];
      ++cptr[vptr[i + 1]];
      ++cptr[vptr[i + 2]];
      ++cptr[vptr[i + 3]];
    }
    for (; i < n; ++i) {
      ++cptr[vptr[i]];
    }

    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, cptr[b]);
    }
  }
  return hash;
}
