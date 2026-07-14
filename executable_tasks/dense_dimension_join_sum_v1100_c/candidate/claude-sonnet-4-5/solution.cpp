#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t probe_size = probe_keys.size();
  const uint32_t* __restrict__ pk = probe_keys.data();
  const uint32_t* __restrict__ pw = probe_weights.data();
  const uint32_t* __restrict__ dv = dim_values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = (probe_size / 4) * 4;
    
    for (; i < unroll_limit; i += 4) {
      __builtin_prefetch(&dv[pk[i + 8]], 0, 0);
      __builtin_prefetch(&dv[pk[i + 9]], 0, 0);
      
      acc0 += static_cast<uint64_t>(dv[pk[i + 0]]) * static_cast<uint64_t>(pw[i + 0]);
      acc1 += static_cast<uint64_t>(dv[pk[i + 1]]) * static_cast<uint64_t>(pw[i + 1]);
      acc2 += static_cast<uint64_t>(dv[pk[i + 2]]) * static_cast<uint64_t>(pw[i + 2]);
      acc3 += static_cast<uint64_t>(dv[pk[i + 3]]) * static_cast<uint64_t>(pw[i + 3]);
    }
    
    for (; i < probe_size; ++i) {
      acc0 += static_cast<uint64_t>(dv[pk[i]]) * static_cast<uint64_t>(pw[i]);
    }
    
    sum = acc0 + acc1 + acc2 + acc3;
  }
  
  return sum;
}