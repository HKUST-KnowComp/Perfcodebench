#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    for (uint32_t v : values) {
      ++counts[static_cast<std::size_t>(v)];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t c : counts) {
      hash = mix(hash, c);
    }
  }
  return hash;
}
