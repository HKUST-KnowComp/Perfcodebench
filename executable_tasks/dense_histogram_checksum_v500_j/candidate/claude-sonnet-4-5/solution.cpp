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
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    memset(counts.data(), 0, bins * sizeof(uint32_t));
    
    // Accumulate histogram with loop unrolling
    size_t i = 0;
    size_t size = values.size();
    size_t unroll_limit = size - (size % 4);
    
    for (; i < unroll_limit; i += 4) {
      ++counts[values[i]];
      ++counts[values[i + 1]];
      ++counts[values[i + 2]];
      ++counts[values[i + 3]];
    }
    
    for (; i < size; ++i) {
      ++counts[values[i]];
    }
    
    // Compute checksum with loop unrolling
    hash = 1469598103934665603ULL;
    uint32_t b = 0;
    uint32_t unroll_bins = bins - (bins % 4);
    
    for (; b < unroll_bins; b += 4) {
      hash = mix(hash, counts[b]);
      hash = mix(hash, counts[b + 1]);
      hash = mix(hash, counts[b + 2]);
      hash = mix(hash, counts[b + 3]);
    }
    
    for (; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  
  return hash;
}