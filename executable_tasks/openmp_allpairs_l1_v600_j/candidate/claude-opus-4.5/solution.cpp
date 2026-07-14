#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const std::size_t n = xs.size();
    if (n <= 1) return 0;
    
    int64_t total = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum_x = 0;
        int64_t sum_y = 0;
        
        // For L1 distance sum: sum over all pairs (i,j) where i<j of |x[i]-x[j]| + |y[i]-y[j]|
        // This equals sum of |x[i]-x[j]| for all pairs + sum of |y[i]-y[j]| for all pairs
        // 
        // For a single coordinate, if we sort the values:
        // sum of |a[i] - a[j]| for i < j = sum over sorted index k of: a[k] * k - prefix_sum[k]
        // where prefix_sum[k] = sum of a[0..k-1]
        
        // Process X coordinates
        {
            std::vector<int32_t> sorted_x(xs);
            std::sort(sorted_x.begin(), sorted_x.end());
            
            int64_t prefix = 0;
            for (std::size_t k = 0; k < n; ++k) {
                int64_t val = sorted_x[k];
                sum_x += val * static_cast<int64_t>(k) - prefix;
                prefix += val;
            }
        }
        
        // Process Y coordinates
        {
            std::vector<int32_t> sorted_y(ys);
            std::sort(sorted_y.begin(), sorted_y.end());
            
            int64_t prefix = 0;
            for (std::size_t k = 0; k < n; ++k) {
                int64_t val = sorted_y[k];
                sum_y += val * static_cast<int64_t>(k) - prefix;
                prefix += val;
            }
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}