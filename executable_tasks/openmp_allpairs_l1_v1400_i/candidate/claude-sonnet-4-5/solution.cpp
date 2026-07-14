#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  int64_t total = 0;
  
  #pragma omp parallel
  {
    int64_t local_sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
      int64_t iter_sum = 0;
      
      #pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        const int32_t xi = xs[i];
        const int32_t yi = ys[i];
        int64_t row_sum = 0;
        
        for (std::size_t j = i + 1; j < n; ++j) {
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
        }
        
        iter_sum += row_sum;
      }
      
      local_sum = iter_sum;
    }
    
    #pragma omp atomic
    total += local_sum;
  }
  
  return total;
}