#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t total_sum = 0;
  const size_t num_runs = values.size();
  
  // Pre-calculate run lengths to avoid repeated subtraction in the loop
  // and to allow for better compiler vectorization/unrolling.
  // However, for maximum performance and minimal memory overhead, 
  // we can just compute it on the fly.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    uint32_t prev_end = 0;
    
    for (size_t i = 0; i < num_runs; ++i) {
      uint32_t current_end = run_ends[i];
      uint32_t length = current_end - prev_end;
      current_iter_sum += static_cast<uint64_t>(length) * values[i];
      prev_end = current_end;
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}