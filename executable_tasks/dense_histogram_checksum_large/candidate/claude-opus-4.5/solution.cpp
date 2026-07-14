#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;
  const uint32_t* data = values.data();
  const size_t n = values.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts.data(), 0, bins * sizeof(uint32_t));
    
    for (size_t i = 0; i < n; ++i) {
      ++counts[data[i]];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  return hash;
}
