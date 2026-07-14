#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    return 1469598103934665603ULL;
  }

  // Expand once and reuse
  std::vector<uint32_t> expanded;
  const std::size_t num_segments = offsets.size() - 1;
  
  // Reserve exact size needed
  if (num_segments > 0) {
    expanded.reserve(offsets.back() - offsets[0]);
  }
  
  for (std::size_t s = 0; s < num_segments; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      expanded.push_back(static_cast<uint32_t>(s));
    }
  }

  uint64_t hash = 0;
  const std::size_t size = expanded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Process in chunks of 4 for better ILP
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      hash = mix(hash, expanded[i]);
      hash = mix(hash, expanded[i + 1]);
      hash = mix(hash, expanded[i + 2]);
      hash = mix(hash, expanded[i + 3]);
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      hash = mix(hash, expanded[i]);
    }
  }
  
  return hash;
}