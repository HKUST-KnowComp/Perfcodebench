#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (keys.empty()) return 0;
  
  // Build O(1) direct lookup table. Dense IDs guarantee max_key is reasonable.
  uint32_t max_key = keys.back();
  std::vector<uint32_t> lookup(max_key + 1);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }
  
  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t* probes_ptr = probes.data();
  size_t n = probes.size();
  
  uint64_t hash = 0;
  const uint64_t prime = 1099511628211ULL;
  const uint64_t offset_basis = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = offset_basis;
    for (size_t i = 0; i < n; ++i) {
      hash ^= lookup_ptr[probes_ptr[i]];
      hash *= prime;
    }
  }
  return hash;
}