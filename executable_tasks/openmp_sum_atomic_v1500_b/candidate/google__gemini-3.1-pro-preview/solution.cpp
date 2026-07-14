#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  if (input.empty()) return 0;
  if (iters <= 0) return 0;

  uint64_t final_sum = 0;
  const std::size_t n = input.size();
  const uint32_t* data = input.data();

  // Move the parallel region outside the iters loop to minimize fork/join overhead.
  // Use reduction to avoid atomic contention on the shared sum variable.
  #pragma omp parallel
  {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local_sum = 0;
      
      // Use static scheduling for predictable memory access patterns and cache efficiency.
      // The compiler can often vectorize this loop automatically.
      #pragma omp for schedule(static) 
      for (std::size_t i = 0; i < n; ++i) {
        local_sum += data[i];
      }

      // The baseline resets 'sum = 0' every iteration and returns the result of the last one.
      // To match this behavior exactly while maintaining performance:
      if (iter == iters - 1) {
        #pragma omp atomic
        final_sum += local_sum;
      } else {
        // Ensure all threads finish the current iteration before starting the next,
        // mimicking the implicit barrier at the end of the baseline's parallel for.
        #pragma omp barrier
      }
    }
  }

  return final_sum;
}