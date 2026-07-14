#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  int64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    
    #pragma omp parallel reduction(+:total)
    {
      int64_t local_sum = 0;
      
      #pragma omp for schedule(dynamic, 64) nowait
      for (std::size_t i = 0; i < n; ++i) {
        const int32_t xi = xs[i];
        const int32_t yi = ys[i];
        int64_t row_sum = 0;
        
        std::size_t j = i + 1;
        const std::size_t limit = n - 3;
        
        // Unrolled loop by 4
        for (; j < limit; j += 4) {
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j+1])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+1]));
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j+2])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+2]));
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j+3])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+3]));
        }
        
        // Handle remaining elements
        for (; j < n; ++j) {
          row_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
        }
        
        local_sum += row_sum;
      }
      
      total += local_sum;
    }
  }
  
  return total;
}