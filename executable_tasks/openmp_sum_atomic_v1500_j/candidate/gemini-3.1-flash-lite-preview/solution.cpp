#include "interface.h"
#include <omp.h>
#include <vector>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t total_sum = 0;
  const size_t n = input.size();
  const uint32_t* data = input.data();

  // Set thread count once to avoid overhead
  int num_threads = std::min(omp_get_max_threads(), 16);
  omp_set_num_threads(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
    #pragma omp parallel reduction(+:iter_sum)
    {
      #pragma omp for schedule(static)
      for (size_t i = 0; i < n; ++i) {
        iter_sum += static_cast<uint64_t>(data[i]);
      }
    }
    total_sum += iter_sum;
  }
  
  return total_sum;
}