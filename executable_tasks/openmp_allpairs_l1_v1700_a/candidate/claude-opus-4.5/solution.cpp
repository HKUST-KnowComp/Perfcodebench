#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  if (n < 2) return 0;
  
  const int32_t* __restrict px = xs.data();
  const int32_t* __restrict py = ys.data();
  
  int64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum = 0;
    
    #pragma omp parallel reduction(+:sum)
    {
      #pragma omp for schedule(dynamic, 16) nowait
      for (std::size_t i = 0; i < n - 1; ++i) {
        const int32_t xi = px[i];
        const int32_t yi = py[i];
        int64_t local_sum = 0;
        
        std::size_t j = i + 1;
        
        // Process elements with SIMD hints
        #pragma omp simd reduction(+:local_sum)
        for (std::size_t jj = j; jj < n; ++jj) {
          int32_t dx = px[jj] - xi;
          int32_t dy = py[jj] - yi;
          // Branchless abs
          int32_t ax = dx < 0 ? -dx : dx;
          int32_t ay = dy < 0 ? -dy : dy;
          local_sum += static_cast<int64_t>(ax) + static_cast<int64_t>(ay);
        }
        
        sum += local_sum;
      }
    }
    
    total = sum;
  }
  
  return total;
}