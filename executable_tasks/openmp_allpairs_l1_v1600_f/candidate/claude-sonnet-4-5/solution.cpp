#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  int64_t total = 0;
  
  const int32_t* __restrict__ xs_ptr = xs.data();
  const int32_t* __restrict__ ys_ptr = ys.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    
    #pragma omp parallel for schedule(dynamic, 64) reduction(+:total)
    for (std::size_t i = 0; i < n; ++i) {
      int64_t local_sum = 0;
      const int32_t xi = xs_ptr[i];
      const int32_t yi = ys_ptr[i];
      
      std::size_t j = i + 1;
      const std::size_t n_unroll = ((n - j) / 4) * 4 + j;
      
      for (; j < n_unroll; j += 4) {
        int64_t dx0 = std::abs(xi - xs_ptr[j]);
        int64_t dy0 = std::abs(yi - ys_ptr[j]);
        int64_t dx1 = std::abs(xi - xs_ptr[j+1]);
        int64_t dy1 = std::abs(yi - ys_ptr[j+1]);
        int64_t dx2 = std::abs(xi - xs_ptr[j+2]);
        int64_t dy2 = std::abs(yi - ys_ptr[j+2]);
        int64_t dx3 = std::abs(xi - xs_ptr[j+3]);
        int64_t dy3 = std::abs(yi - ys_ptr[j+3]);
        
        local_sum += dx0 + dy0 + dx1 + dy1 + dx2 + dy2 + dx3 + dy3;
      }
      
      for (; j < n; ++j) {
        local_sum += static_cast<int64_t>(std::abs(xi - xs_ptr[j])) +
                     static_cast<int64_t>(std::abs(yi - ys_ptr[j]));
      }
      
      total += local_sum;
    }
  }
  
  return total;
}