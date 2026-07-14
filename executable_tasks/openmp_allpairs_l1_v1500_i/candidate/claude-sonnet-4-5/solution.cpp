#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  int64_t total = 0;
  const std::size_t n = xs.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    
    #pragma omp parallel for schedule(dynamic, 64) reduction(+:total)
    for (std::size_t i = 0; i < n; ++i) {
      int64_t local_sum = 0;
      const int32_t xi = xs[i];
      const int32_t yi = ys[i];
      
      std::size_t j = i + 1;
      const std::size_t end = n;
      const std::size_t end_unroll = i + 1 + ((end - i - 1) / 4) * 4;
      
      // Unrolled loop
      for (; j < end_unroll; j += 4) {
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+1])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+1]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+2])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+2]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+3])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+3]));
      }
      
      // Remainder loop
      for (; j < end; ++j) {
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
      }
      
      total += local_sum;
    }
  }
  return total;
}