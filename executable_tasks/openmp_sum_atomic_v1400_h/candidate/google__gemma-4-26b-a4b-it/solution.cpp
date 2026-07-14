#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int get_optimal_threads() {
  // Limit threads to a reasonable number to avoid overhead if the input is small,
  // but allow scaling up to 8 as per the original logic.
  return std::max(1, std::min(8, omp_get_max_threads()));
}

} // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const int num_threads = get_optimal_threads();
  omp_set_num_threads(num_threads);
  
  const std::size_t n = input.size();
  uint64_t total_sum = 0;

  // We move the iteration loop outside or inside depending on the goal.
  // Since the task asks to sum the buffer 'repeatedly', we must ensure
  // the total sum accumulates across all iterations.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
    // Using reduction(+:iter_sum) is the most efficient way to perform
    // a parallel sum in OpenMP. It avoids the atomic contention bottleneck.
    #pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(input[i]);
    }
    total_sum += iter_sum;
  }

  return total_sum;
}
