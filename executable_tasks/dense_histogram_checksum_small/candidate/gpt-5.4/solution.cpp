#include "interface.h"

#include <algorithm>
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
    std::fill(counts.begin(), counts.end(), 0u);

    for (uint32_t v : values) {
      ++counts[v];
    }

    hash = 1469598103934665603ULL;
    const uint32_t* p = counts.data();
    const uint32_t* end = p + counts.size();
    while (p != end) {
      hash = mix(hash, *p++);
    }
  }

  return hash;
}
