#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const int n = static_cast<int>(xs.size());
    if (n <= 1) return 0;
    
    int64_t total = 0;
    
    // For sum of |a_i - a_j| over all i < j:
    // Sort the array. Then for sorted value v[k], it contributes:
    // v[k] * (2*k - n + 1) to the total sum
    // This is because v[k] is subtracted by k elements before it
    // and added by (n-1-k) elements after it.
    // Net contribution: v[k] * ((n-1-k) - k) = v[k] * (n - 1 - 2k)
    // But we want sum of |v[i] - v[j]| for i < j, which equals:
    // sum over k of v[k] * (2*k - (n-1))
    
    std::vector<int32_t> sorted_x(n);
    std::vector<int32_t> sorted_y(n);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Copy and sort both arrays
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                std::copy(xs.begin(), xs.end(), sorted_x.begin());
                std::sort(sorted_x.begin(), sorted_x.end());
            }
            #pragma omp section
            {
                std::copy(ys.begin(), ys.end(), sorted_y.begin());
                std::sort(sorted_y.begin(), sorted_y.end());
            }
        }
        
        int64_t sum_x = 0;
        int64_t sum_y = 0;
        
        // Compute sum using the formula:
        // For sorted array, sum of |a[i] - a[j]| for i < j equals
        // sum over k of a[k] * (2*k - (n-1))
        #pragma omp parallel reduction(+:sum_x, sum_y)
        {
            #pragma omp for nowait
            for (int k = 0; k < n; ++k) {
                int64_t coeff = 2LL * k - (n - 1);
                sum_x += static_cast<int64_t>(sorted_x[k]) * coeff;
            }
            
            #pragma omp for
            for (int k = 0; k < n; ++k) {
                int64_t coeff = 2LL * k - (n - 1);
                sum_y += static_cast<int64_t>(sorted_y[k]) * coeff;
            }
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}