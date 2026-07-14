#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>

namespace {

int num_threads() {
  return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const int nt = num_threads();
  omp_set_num_threads(nt);
  
  uint64_t sum = 0;
  const std::size_t n = input.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<uint64_t> local_sums(nt, 0);
    
#pragma omp parallel
    {
      const int tid = omp_get_thread_num();
      uint64_t local_sum = 0;
      
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        local_sum += static_cast<uint64_t>(input[i]);
      }
      
      local_sums[tid] = local_sum;
    }
    
    sum = 0;
    for (int t = 0; t < nt; ++t) {
      sum += local_sums[t];
    }
  }
  
  return sum;
}