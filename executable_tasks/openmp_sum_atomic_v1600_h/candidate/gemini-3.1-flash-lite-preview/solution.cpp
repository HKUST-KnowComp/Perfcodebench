#include "interface.h"
#include <algorithm>
#include <omp.h>
#include <vector>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t total_sum = 0;
  const size_t n = input.size();
  const uint32_t* data = input.data();

  // Use a fixed number of threads to avoid overhead
  int num_threads = std::min(omp_get_max_threads(), 16);
  omp_set_num_threads(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;

    #pragma omp parallel
    {
      uint64_t local_sum = 0;
      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        local_sum += data[i];
      }

      #pragma omp atomic
      iter_sum += local_sum;
    }
    total_sum += iter_sum;
  }
  return total_sum;
}