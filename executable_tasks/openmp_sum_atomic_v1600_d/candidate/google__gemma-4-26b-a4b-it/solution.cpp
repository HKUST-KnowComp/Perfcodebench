#include "interface.h"

#include <algorithm>
#include <omp.h>

namespace {

int get_optimal_threads() {
  // Limit threads to a reasonable number to avoid overhead if the input is small,
  // but allow scaling for large buffers.
  int max_t = omp_get_max_threads();
  return std::max(1, std::min(max_t, 16));
}

} // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  const uint32_t* data = input.data();
  uint64_t total_sum = 0;

  // We move the iteration loop outside or inside depending on the goal.
  // Since the task asks to sum the buffer 'repeatedly', we must ensure
  // the total sum is accumulated across all iterations.
  
  // Optimization: Use OpenMP reduction clause to avoid atomic contention.
  // This allows each thread to work on a private partial sum.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    #pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(data[i]);
    }
    total_sum += iter_sum;
  }

  return total_sum;
}
