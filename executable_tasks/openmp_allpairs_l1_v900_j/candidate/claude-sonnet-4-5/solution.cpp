#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  int64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum = 0;
    
    #pragma omp parallel for schedule(dynamic, 64) reduction(+:sum)
    for (std::size_t i = 0; i < n; ++i) {
      const int32_t xi = xs[i];
      const int32_t yi = ys[i];
      int64_t local_sum = 0;
      
      std::size_t j = i + 1;
      
      // Unrolled loop for better ILP
      for (; j + 3 < n; j += 4) {
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+1])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+1]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+2])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+2]));
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j+3])) +
                     static_cast<int64_t>(std::abs(yi - ys[j+3]));
      }
      
      // Handle remaining elements
      for (; j < n; ++j) {
        local_sum += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
      }
      
      sum += local_sum;
    }
    
    total = sum;
  }
  
  return total;
}