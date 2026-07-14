#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* __restrict__ data = input.data();
  const std::size_t n = input.size();
  
  int nthreads = omp_get_max_threads();
  if (nthreads > 16) nthreads = 16;
  if (nthreads < 1) nthreads = 1;
  omp_set_num_threads(nthreads);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
#pragma omp parallel for reduction(+:local_sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      local_sum += data[i];
    }
    
    sum = local_sum;
  }
  
  return sum;
}