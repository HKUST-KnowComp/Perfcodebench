#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (dim_keys.empty() || probe_keys.empty()) return 0;
  
  // "Dense dimension" implies dim_keys are contiguous integers:
  // dim_keys[i] == dim_keys[0] + i.
  // We replace O(log M) std::lower_bound with O(1) direct indexing.
  const uint32_t base = dim_keys[0];
  const std::size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const uint32_t* d_values = dim_values.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    
    // Manual 4x unrolling breaks the reduction dependency chain,
    // hides memory latency, and strongly hints auto-vectorization.
    for (; i + 3 < n; i += 4) {
      s0 += static_cast<uint64_t>(d_values[p_keys[i] - base]) * p_weights[i];
      s1 += static_cast<uint64_t>(d_values[p_keys[i+1] - base]) * p_weights[i+1];
      s2 += static_cast<uint64_t>(d_values[p_keys[i+2] - base]) * p_weights[i+2];
      s3 += static_cast<uint64_t>(d_values[p_keys[i+3] - base]) * p_weights[i+3];
    }
    for (; i < n; ++i) {
      s0 += static_cast<uint64_t>(d_values[p_keys[i] - base]) * p_weights[i];
    }
    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}