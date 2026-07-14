#include "interface.h"

#include <vector>
#include <cstdint>

/**
 * Optimized delta_prefix_sum implementation.
 * 
 * Mathematical derivation:
 * Let d[i] be the deltas and b be the base.
 * The decoded values are:
 * v[0] = b + d[0]
 * v[1] = b + d[0] + d[1]
 * v[2] = b + d[0] + d[1] + d[2]
 * ...
 * v[i] = b + sum(d[0...i])
 * 
 * The total sum is:
 * S = sum_{i=0 to N-1} v[i]
 * S = sum_{i=0 to N-1} (b + sum_{j=0 to i} d[j])
 * S = N * b + sum_{i=0 to N-1} sum_{j=0 to i} d[j]
 * 
 * In the double summation, each d[j] appears in v[j], v[j+1], ..., v[N-1].
 * There are exactly (N - j) such terms.
 * 
 * S = N * b + sum_{j=0 to N-1} (d[j] * (N - j))
 */
uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const size_t n = deltas.size();
  if (n == 0) return 0;

  uint64_t final_sum = 0;
  
  // The baseline resets 'sum' inside the 'iters' loop, so only the last iteration matters.
  // However, we execute the logic inside the loop to ensure we respect the benchmark structure.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = (uint64_t)n * base;
    
    // Use a pointer for potentially better vectorization hints to the compiler
    const uint32_t* __restrict d_ptr = deltas.data();
    
    // Single pass calculation: O(N) time, O(1) space
    for (size_t j = 0; j < n; ++j) {
      current_sum += (uint64_t)d_ptr[j] * (n - j);
    }
    
    final_sum = current_sum;
  }

  return final_sum;
}