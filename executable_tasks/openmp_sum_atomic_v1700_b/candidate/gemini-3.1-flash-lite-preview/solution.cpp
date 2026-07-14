#include "interface.h"
#include <omp.h>
#include <vector>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  uint64_t total_sum = 0;
  const size_t n = input.size();
  const uint32_t* data = input.data();

  // Use the maximum available threads for optimal throughput
  int max_threads = omp_get_max_threads();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
    #pragma omp parallel num_threads(max_threads)
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