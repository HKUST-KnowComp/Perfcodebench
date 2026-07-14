#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <cstdint>
#include <vector>

/**
 * Performance Engineer's Optimization Notes:
 * 1. Removed #pragma omp atomic: The original code performed an atomic operation for every single element,
 *    causing massive cache contention (cache line bouncing) and serialization across threads.
 * 2. Used reduction(+:sum): OpenMP's reduction clause is the idiomatic and most efficient way to sum
 *    elements. It allows each thread to accumulate a local sum in a register or private cache line,
 *    combining them only at the end of the loop.
 * 3. Pointer Aliasing: Used a raw pointer (input.data()) to ensure the compiler can perform optimal
 *    vectorization without worrying about potential vector object mutations.
 * 4. Thread Count: Removed the arbitrary limit of 8 threads to allow the implementation to scale
 *    to the full capability of the hardware.
 * 5. Loop Scheduling: Used 'schedule(static)' which is ideal for balanced workloads like array
 *    traversals, minimizing scheduling overhead.
 */

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* __restrict data = input.data();
  const std::size_t n = input.size();
  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    
    // Using reduction(+:current_iter_sum) is significantly faster than atomic
    // because it eliminates cross-thread communication during the hot loop.
    #pragma omp parallel for reduction(+:current_iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      current_iter_sum += static_cast<uint64_t>(data[i]);
    }
    
    final_sum = current_iter_sum;
  }

  return final_sum;
}