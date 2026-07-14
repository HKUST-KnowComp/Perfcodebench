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
  std::vector<uint32_t> counts(bins);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(counts.data(), 0, bins * sizeof(uint32_t));
    
    // Accumulate histogram with loop unrolling
    const uint32_t* data = values.data();
    size_t size = values.size();
    size_t i = 0;
    
    // Process 4 elements at a time
    for (; i + 3 < size; i += 4) {
      ++counts[data[i]];
      ++counts[data[i + 1]];
      ++counts[data[i + 2]];
      ++counts[data[i + 3]];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      ++counts[data[i]];
    }
    
    // Compute checksum
    hash = 1469598103934665603ULL;
    const uint32_t* counts_data = counts.data();
    
    // Unroll checksum computation
    uint32_t b = 0;
    for (; b + 3 < bins; b += 4) {
      hash = mix(hash, counts_data[b]);
      hash = mix(hash, counts_data[b + 1]);
      hash = mix(hash, counts_data[b + 2]);
      hash = mix(hash, counts_data[b + 3]);
    }
    
    for (; b < bins; ++b) {
      hash = mix(hash, counts_data[b]);
    }
  }
  
  return hash;
}