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
      #pragma omp for schedule(static) nowait
      for (std::size_t i = 0; i < n; ++i) {
        int64_t local_sum = 0;
        const int32_t xi = xs[i];
        const int32_t yi = ys[i];
        
        std::size_t j = i + 1;
        const std::size_t end = n;
        const std::size_t end_unroll = i + 1 + ((end - i - 1) / 4) * 4;
        
        // Unrolled loop
        for (; j < end_unroll; j += 4) {
          int32_t dx0 = xi - xs[j];
          int32_t dy0 = yi - ys[j];
          int32_t dx1 = xi - xs[j+1];
          int32_t dy1 = yi - ys[j+1];
          int32_t dx2 = xi - xs[j+2];
          int32_t dy2 = yi - ys[j+2];
          int32_t dx3 = xi - xs[j+3];
          int32_t dy3 = yi - ys[j+3];
          
          local_sum += (dx0 >= 0 ? dx0 : -dx0) + (dy0 >= 0 ? dy0 : -dy0);
          local_sum += (dx1 >= 0 ? dx1 : -dx1) + (dy1 >= 0 ? dy1 : -dy1);
          local_sum += (dx2 >= 0 ? dx2 : -dx2) + (dy2 >= 0 ? dy2 : -dy2);
          local_sum += (dx3 >= 0 ? dx3 : -dx3) + (dy3 >= 0 ? dy3 : -dy3);
        }
        
        // Remainder loop
        for (; j < end; ++j) {
          int32_t dx = xi - xs[j];
          int32_t dy = yi - ys[j];
          local_sum += (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
        }
        
        total += local_sum;
      }
    }
  }
  
  return total;
}