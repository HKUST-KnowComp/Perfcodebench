#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
  const std::size_t n = xs.size();
  const int32_t* x_data = xs.data();
  const int32_t* y_data = ys.data();
  
  int64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t sum = 0;
    
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
      int64_t local_sum = 0;
      const int32_t xi = x_data[i];
      const int32_t yi = y_data[i];
      
      for (std::size_t j = i + 1; j < n; ++j) {
        local_sum += static_cast<int64_t>(std::abs(xi - x_data[j])) +
                     static_cast<int64_t>(std::abs(yi - y_data[j]));
      }
      sum += local_sum;
    }
    
    total = sum;
  }
  
  return total;
}