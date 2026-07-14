#include "interface.h"
#include <algorithm>
#include <omp.h>

namespace {

int get_optimal_threads() {
  // Limit threads to a reasonable number to avoid oversubscription overhead
  // while ensuring we utilize available hardware concurrency.
  int max_t = omp_get_max_threads();
  return std::max(1, std::min(8, max_t));
}

} // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const std::size_t n = input.size();
  const uint32_t* data = input.data();
  int threads = get_optimal_threads();
  omp_set_num_threads(threads);

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    // Using reduction clause is significantly faster than atomic because it
    // eliminates cache line bouncing/contention between threads.
    // The compiler can also vectorize the loop more effectively.
    #pragma omp parallel for reduction(+:iter_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(data[i]);
    }
    total_sum = iter_sum;
  }

  return total_sum;
}