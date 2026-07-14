#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* __restrict__ data = input.data();
  const std::size_t n = input.size();
  const int nthreads = omp_get_max_threads();
  omp_set_num_threads(nthreads);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
#pragma omp parallel reduction(+:iter_sum)
    {
      uint64_t local_sum = 0;
      
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local_sum += data[i];
      }
      
      iter_sum += local_sum;
    }
    
    sum = iter_sum;
  }
  
  return sum;
}