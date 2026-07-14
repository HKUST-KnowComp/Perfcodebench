#include "interface.h"
#include <algorithm>
#include <omp.h>

namespace {

int get_optimal_threads() {
  // Limit threads to a reasonable number to avoid oversubscription overhead,
  // but allow scaling up to the hardware capability.
  int max_t = omp_get_max_threads();
  return std::max(1, std::min(max_t, 16));
}

} // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  const uint32_t* data = input.data();
  uint64_t total_sum = 0;

  // We move the thread setting outside the loop to avoid repeated overhead.
  // However, to strictly follow the pattern of the original, we ensure the parallel region
  // is optimized. The most significant bottleneck was the atomic update.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
    // Using reduction(+:iter_sum) is the standard, high-performance way to perform 
    // parallel summation in OpenMP. It eliminates the atomic contention.
    #pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(data[i]);
    }
    
    // Note: The task asks for the sum of the buffer repeatedly. 
    // The original code reset 'sum' every iteration and returned the last iteration's sum.
    // To preserve the exact behavior of the baseline (returning the sum of the last iteration):
    total_sum = iter_sum;
  }

  return total_sum;
}