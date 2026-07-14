#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const size_t n = keys.size();
  const uint32_t* k_ptr = keys.data();
  const uint32_t* v_ptr = values.data();
  const uint8_t* f_ptr = flags.data();
  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t i = 0;
    
    // Unroll loop to improve ILP and help the compiler vectorize
    for (; i + 3 < n; i += 4) {
      local_sum += (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3u) == 0u) ? v_ptr[i] : 0;
      local_sum += (f_ptr[i+1] && k_ptr[i+1] < key_threshold && (k_ptr[i+1] & 3u) == 0u) ? v_ptr[i+1] : 0;
      local_sum += (f_ptr[i+2] && k_ptr[i+2] < key_threshold && (k_ptr[i+2] & 3u) == 0u) ? v_ptr[i+2] : 0;
      local_sum += (f_ptr[i+3] && k_ptr[i+3] < key_threshold && (k_ptr[i+3] & 3u) == 0u) ? v_ptr[i+3] : 0;
    }
    
    for (; i < n; ++i) {
      if (f_ptr[i] && k_ptr[i] < key_threshold && (k_ptr[i] & 3u) == 0u) {
        local_sum += v_ptr[i];
      }
    }
    total_sum = local_sum;
  }
  
  return total_sum;
}