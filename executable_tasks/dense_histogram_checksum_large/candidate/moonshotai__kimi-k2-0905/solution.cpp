#include "interface.h"
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins,
                            int iters) {
  static thread_local std::vector<uint32_t> counts;
  if (counts.size() < bins) {
    counts.resize(bins);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts.data(), 0, bins * sizeof(uint32_t));

    for (uint32_t v : values) {
      if (v < bins) {
        ++counts[v];
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}