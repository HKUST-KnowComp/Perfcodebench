#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(static_cast<size_t>(bins), 0u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (bins != 0) {
      counts.assign(static_cast<size_t>(bins), 0u);

      for (uint32_t v : values) {
        if (v < bins) {
          ++counts[static_cast<size_t>(v)];
        }
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[static_cast<size_t>(b)]);
    }
  }

  return hash;
}
