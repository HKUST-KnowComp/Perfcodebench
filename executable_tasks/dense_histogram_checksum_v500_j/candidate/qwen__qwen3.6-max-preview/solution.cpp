#include "interface.h"
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
  uint32_t* counts_data = counts.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts_data, 0, bins * sizeof(uint32_t));
    
    for (uint32_t v : values) {
      if (v < bins) {
        ++counts_data[v];
      }
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts_data[b]);
    }
  }
  return hash;
}