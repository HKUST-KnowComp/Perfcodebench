#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const size_t n = xs.size();
    if (n <= 1) return 0;
    
    int64_t total = 0;
    
    // Make copies for sorting
    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset and copy for each iteration
        if (iter > 0) {
            std::copy(xs.begin(), xs.end(), sx.begin());
            std::copy(ys.begin(), ys.end(), sy.begin());
        }
        
        // Sort both coordinate arrays
        std::sort(sx.begin(), sx.end());
        std::sort(sy.begin(), sy.end());
        
        // For sorted array, sum of |a[i] - a[j]| for all i < j equals:
        // sum over i of: a[i] * (2*i - n + 1)
        // This is because each element a[i] appears in (n-1-i) pairs where it's the larger
        // and in i pairs where it's the smaller.
        // Contribution = a[i] * (n-1-i) - a[i] * i = a[i] * (n - 1 - 2*i)
        // But we want sum for i < j, so: a[i] contributes -a[i] for each j > i (subtracted)
        // and a[j] contributes +a[j] for each i < j (added)
        // Total = sum_i [ a[i] * i - a[i] * (n-1-i) ] = sum_i [ a[i] * (2*i - n + 1) ]
        
        int64_t sum_x = 0;
        int64_t sum_y = 0;
        
        #pragma omp parallel for reduction(+:sum_x, sum_y) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            int64_t coeff = static_cast<int64_t>(2 * i) - static_cast<int64_t>(n) + 1;
            sum_x += static_cast<int64_t>(sx[i]) * coeff;
            sum_y += static_cast<int64_t>(sy[i]) * coeff;
        }
        
        total = sum_x + sum_y;
    }
    
    return total;
}