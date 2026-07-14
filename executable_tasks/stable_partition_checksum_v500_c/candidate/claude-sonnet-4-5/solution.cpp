#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  std::vector<uint32_t> out;
  out.reserve(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    
    // First pass: collect flagged values and hash them
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      if (__builtin_expect(flags[i], 1)) {
        uint32_t v = values[i];
        out.push_back(v);
        hash = mix(hash, v);
      }
    }
    
    // Second pass: collect non-flagged values and hash them
    for (std::size_t i = 0; i < n; ++i) {
      if (__builtin_expect(!flags[i], 0)) {
        uint32_t v = values[i];
        out.push_back(v);
        hash = mix(hash, v);
      }
    }
  }
  
  return hash;
}