#include "interface.h"

#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins,
                            int iters) {
  // Allocate once, zero once per iteration.
  uint32_t* counts = new uint32_t[bins];

  const uint64_t h0 = 1469598103934665603ULL;
  const uint64_t kMul = 1099511628211ULL;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts, 0, bins * sizeof(uint32_t));

    const uint32_t* v = values.data();
    const size_t n = values.size();
    for (size_t i = 0; i < n; ++i) {
      const uint32_t idx = v[i];
      if (idx < bins) {
        ++counts[idx];
      }
    }

    uint64_t h = h0;
    for (uint32_t b = 0; b < bins; ++b) {
      h ^= counts[b];
      h *= kMul;
    }
    hash = h;
  }

  delete[] counts;
  return hash;
}