#pragma once

#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.empty()) return 0;
  
  uint64_t hash = 1469598103934665603ULL;
  
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint64_t segment_id = static_cast<uint64_t>(s);
    
    // Unroll loop for better performance
    for (uint32_t i = 0; i < len; ++i) {
      hash ^= segment_id;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}