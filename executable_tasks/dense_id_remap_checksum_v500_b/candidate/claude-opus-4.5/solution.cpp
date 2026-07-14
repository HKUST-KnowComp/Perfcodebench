#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 0;
  }

  // Find min and max keys to build a direct lookup table
  uint32_t min_key = keys[0];
  uint32_t max_key = keys[0];
  for (size_t i = 1; i < keys.size(); ++i) {
    if (keys[i] < min_key) min_key = keys[i];
    if (keys[i] > max_key) max_key = keys[i];
  }

  // Build direct lookup table: table[key - min_key] = value
  const size_t table_size = static_cast<size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> table(table_size);
  for (size_t i = 0; i < keys.size(); ++i) {
    table[keys[i] - min_key] = values[i];
  }

  const uint32_t* __restrict table_ptr = table.data();
  const uint32_t* __restrict probes_ptr = probes.data();
  const size_t probes_size = probes.size();

  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    const size_t unroll_limit = probes_size & ~size_t(3);
    
    // Unrolled loop - process 4 probes at a time
    for (; i < unroll_limit; i += 4) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      uint64_t v0 = table_ptr[p0 - min_key];
      uint64_t v1 = table_ptr[p1 - min_key];
      uint64_t v2 = table_ptr[p2 - min_key];
      uint64_t v3 = table_ptr[p3 - min_key];
      
      hash ^= v0;
      hash *= 1099511628211ULL;
      hash ^= v1;
      hash *= 1099511628211ULL;
      hash ^= v2;
      hash *= 1099511628211ULL;
      hash ^= v3;
      hash *= 1099511628211ULL;
    }
    
    // Handle remaining probes
    for (; i < probes_size; ++i) {
      uint64_t v = table_ptr[probes_ptr[i] - min_key];
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}