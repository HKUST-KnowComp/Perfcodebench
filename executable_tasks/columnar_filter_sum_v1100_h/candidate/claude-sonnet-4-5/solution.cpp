#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ keys_ptr = keys.data();
  const uint32_t* __restrict__ values_ptr = values.data();
  const uint8_t* __restrict__ flags_ptr = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Main vectorizable loop
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t key = keys_ptr[i];
      uint32_t value = values_ptr[i];
      uint8_t flag = flags_ptr[i];
      
      // Combine all predicates into a single condition
      // This reduces branches and enables better vectorization
      bool predicate = flag && (key < key_threshold) && ((key & 3u) == 0u);
      
      // Branchless accumulation using conditional move semantics
      local_sum += predicate ? static_cast<uint64_t>(value) : 0ULL;
    }
    
    sum = local_sum;
  }
  
  return sum;
}