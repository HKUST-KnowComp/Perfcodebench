#include "interface.h"

#include <unordered_map>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::unordered_map<uint32_t, uint32_t> counts;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    counts.clear();
    for (uint32_t v : values) {
      ++counts[v];
    }
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      const auto it = counts.find(b);
      hash = mix(hash, it == counts.end() ? 0u : it->second);
    }
  }
  return hash;
}
