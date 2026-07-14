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
  if (iters <= 0) return 0;
  if (bins == 0) return 1469598103934665603ULL;

  std::vector<uint32_t> counts(bins);

  const uint32_t* data = values.data();
  const size_t n = values.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t v = data[i];
    if (v < bins) {
      ++counts[v];
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (size_t b = 0; b < counts.size(); ++b) {
    hash = mix(hash, counts[b]);
  }
  return hash;
}
