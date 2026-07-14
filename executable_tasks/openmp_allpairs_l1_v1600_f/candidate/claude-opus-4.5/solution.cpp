#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    const size_t n = xs.size();
    
    if (n <= 1) {
        return 0;
    }
    
    // For sum of |a[i] - a[j]| over all i < j:
    // Sort the array, then sum[i] = a[i] * (2*i - n + 1) for i in 0..n-1
    // This works because after sorting, for element at position i:
    // - There are i elements smaller (each contributes +a[i])
    // - There are (n-1-i) elements larger (each contributes -a[i])
    // - Net contribution: a[i] * (i - (n-1-i)) = a[i] * (2*i - n + 1)
    
    std::vector<int32_t> sorted_xs(n);
    std::vector<int32_t> sorted_ys(n);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy and sort both arrays in parallel
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                std::copy(xs.begin(), xs.end(), sorted_xs.begin());
                std::sort(sorted_xs.begin(), sorted_xs.end());
            }
            #pragma omp section
            {
                std::copy(ys.begin(), ys.end(), sorted_ys.begin());
                std::sort(sorted_ys.begin(), sorted_ys.end());
            }
        }
        
        int64_t sum_x = 0;
        int64_t sum_y = 0;
        
        // Compute the weighted sums in parallel
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                int64_t local_sum = 0;
                const int64_t n_val = static_cast<int64_t>(n);
                for (size_t i = 0; i < n; ++i) {
                    int64_t weight = 2 * static_cast<int64_t>(i) - n_val + 1;
                    local_sum += static_cast<int64_t>(sorted_xs[i]) * weight;
                }
                sum_x = local_sum;
            }
            #pragma omp section
            {
                int64_t local_sum = 0;
                const int64_t n_val = static_cast<int64_t>(n);
                for (size_t i = 0; i < n; ++i) {
                    int64_t weight = 2 * static_cast<int64_t>(i) - n_val + 1;
                    local_sum += static_cast<int64_t>(sorted_ys[i]) * weight;
                }
                sum_y = local_sum;
            }
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}