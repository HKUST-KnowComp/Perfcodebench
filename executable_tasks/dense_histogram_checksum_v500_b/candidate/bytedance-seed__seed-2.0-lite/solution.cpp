#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t v : values) {
      ++counts[v];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      const uint32_t c = counts[b];
      hash = mix(hash, c);
      counts[b] = 0;
    }
  }
  return hash;
}