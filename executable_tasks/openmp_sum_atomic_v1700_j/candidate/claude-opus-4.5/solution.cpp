#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
  const uint32_t* __restrict__ data = input.data();
  const std::size_t n = input.size();
  const std::size_t n4 = n & ~3ULL;
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    
#pragma omp parallel reduction(+:iter_sum)
    {
#pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n4; i += 4) {
        iter_sum += static_cast<uint64_t>(data[i]) +
                    static_cast<uint64_t>(data[i + 1]) +
                    static_cast<uint64_t>(data[i + 2]) +
                    static_cast<uint64_t>(data[i + 3]);
      }
      
#pragma omp single
      {
        for (std::size_t i = n4; i < n; ++i) {
          iter_sum += static_cast<uint64_t>(data[i]);
        }
      }
    }
    
    sum = iter_sum;
  }
  
  return sum;
}