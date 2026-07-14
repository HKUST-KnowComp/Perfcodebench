#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts.data(), 0, bins * sizeof(uint32_t));
    
    // Accumulate histogram with loop unrolling
    const size_t size = values.size();
    size_t i = 0;
    
    // Process 4 elements at a time
    for (; i + 3 < size; i += 4) {
      ++counts[values[i]];
      ++counts[values[i + 1]];
      ++counts[values[i + 2]];
      ++counts[values[i + 3]];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      ++counts[values[i]];
    }
    
    // Compute checksum
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      hash = mix(hash, counts[b]);
    }
  }
  
  return hash;
}