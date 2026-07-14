#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

static int64_t compute_l1_sum_1d(std::vector<int32_t>& vals) {
    const int n = static_cast<int>(vals.size());
    if (n <= 1) return 0;
    
    std::sort(vals.begin(), vals.end());
    
    int64_t total = 0;
    int64_t prefix_sum = 0;
    
    for (int i = 0; i < n; ++i) {
        int64_t v = vals[i];
        // Contribution: v * i - prefix_sum (sum of distances to all previous elements)
        total += v * i - prefix_sum;
        prefix_sum += v;
    }
    
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        std::vector<int32_t> xs_copy = xs;
        std::vector<int32_t> ys_copy = ys;
        
        int64_t sum_x = 0, sum_y = 0;
        
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                sum_x = compute_l1_sum_1d(xs_copy);
            }
            #pragma omp section
            {
                sum_y = compute_l1_sum_1d(ys_copy);
            }
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}