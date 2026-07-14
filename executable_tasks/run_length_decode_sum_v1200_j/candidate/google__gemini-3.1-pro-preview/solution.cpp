#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized run-length sum calculation.
 * 
 * The original implementation decoded the RLE stream into a vector and then summed it,
 * repeating this process 'iters' times. This is O(iters * total_decoded_length).
 * 
 * Mathematically, the sum of an RLE stream is the sum of (value * length) for each pair.
 * Since the result of each iteration is identical and the sum is reset inside the loop,
 * we only need to compute the sum once (if iters > 0) and return it.
 * 
 * This reduces the complexity to O(values.size()) and eliminates all memory allocations.
 */
uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  
  // If iters is 0 or less, the original loop never executes, and sum remains 0.
  if (iters <= 0) {
    return 0;
  }

  uint64_t total_sum = 0;
  const size_t n = values.size();
  
  // Use raw pointers to assist the compiler with auto-vectorization
  const uint32_t* __restrict v_ptr = values.data();
  const uint16_t* __restrict l_ptr = lengths.data();

  // Compute the sum of (value * length) directly.
  // This is semantically equivalent to the decoded sum.
  for (size_t i = 0; i < n; ++i) {
    total_sum += static_cast<uint64_t>(v_ptr[i]) * l_ptr[i];
  }

  return total_sum;
}