#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const int n = static_cast<int>(xs.size());
    if (n <= 1) return 0;
    
    int64_t total = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum_x = 0;
        int64_t sum_y = 0;
        
        #pragma omp parallel sections reduction(+:sum_x,sum_y)
        {
            #pragma omp section
            {
                std::vector<int32_t> sorted_x(xs);
                std::sort(sorted_x.begin(), sorted_x.end());
                
                int64_t prefix = 0;
                for (int i = 0; i < n; ++i) {
                    int64_t val = sorted_x[i];
                    sum_x += val * (2LL * i - n + 1) + (static_cast<int64_t>(n - 1 - i) - i) * val - prefix + (prefix - val * i) + val * i;
                }
                
                sum_x = 0;
                prefix = 0;
                for (int i = 0; i < n; ++i) {
                    int64_t val = sorted_x[i];
                    sum_x += val * i - prefix;
                    prefix += val;
                }
            }
            
            #pragma omp section
            {
                std::vector<int32_t> sorted_y(ys);
                std::sort(sorted_y.begin(), sorted_y.end());
                
                int64_t prefix = 0;
                for (int i = 0; i < n; ++i) {
                    int64_t val = sorted_y[i];
                    sum_y += val * i - prefix;
                    prefix += val;
                }
            }
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}