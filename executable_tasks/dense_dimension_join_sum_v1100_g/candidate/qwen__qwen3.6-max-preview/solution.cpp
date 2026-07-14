#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (dim_keys.empty() || probe_keys.empty()) return 0;
  
  const uint32_t offset = dim_keys[0];
  const uint32_t* dv = dim_values.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  const size_t n = probe_keys.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      s0 += static_cast<uint64_t>(dv[pk[i] - offset]) * pw[i];
      s1 += static_cast<uint64_t>(dv[pk[i+1] - offset]) * pw[i+1];
      s2 += static_cast<uint64_t>(dv[pk[i+2] - offset]) * pw[i+2];
      s3 += static_cast<uint64_t>(dv[pk[i+3] - offset]) * pw[i+3];
    }
    sum = s0 + s1 + s2 + s3;
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(dv[pk[i] - offset]) * pw[i];
    }
  }
  return sum;
}
