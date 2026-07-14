#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  // Find min and max keys to build a direct lookup table
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] < min_key) min_key = dim_keys[i];
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Build direct lookup table: lookup[key - min_key] = value
  const std::size_t table_size = static_cast<std::size_t>(max_key - min_key + 1);
  std::vector<uint32_t> lookup(table_size);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  const uint32_t* __restrict__ lookup_ptr = lookup.data();
  const uint32_t* __restrict__ pk = probe_keys.data();
  const uint32_t* __restrict__ pw = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    
    for (; i < n4; i += 4) {
      const uint32_t k0 = pk[i]     - min_key;
      const uint32_t k1 = pk[i + 1] - min_key;
      const uint32_t k2 = pk[i + 2] - min_key;
      const uint32_t k3 = pk[i + 3] - min_key;
      
      s0 += static_cast<uint64_t>(lookup_ptr[k0]) * static_cast<uint64_t>(pw[i]);
      s1 += static_cast<uint64_t>(lookup_ptr[k1]) * static_cast<uint64_t>(pw[i + 1]);
      s2 += static_cast<uint64_t>(lookup_ptr[k2]) * static_cast<uint64_t>(pw[i + 2]);
      s3 += static_cast<uint64_t>(lookup_ptr[k3]) * static_cast<uint64_t>(pw[i + 3]);
    }
    
    for (; i < n; ++i) {
      s0 += static_cast<uint64_t>(lookup_ptr[pk[i] - min_key]) * static_cast<uint64_t>(pw[i]);
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}