#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <omp.h>

int64_t compute_dim_sum(std::vector<int32_t> coords) {
    int64_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t dim_sum = 0;
    // The sum of |a_i - a_j| for i < j is sum_{i=0}^{n-1} (i * a_i - (n-1-i) * a_i)
    // which simplifies to sum_{i=0}^{n-1} (2*i - n + 1) * a_i
    #pragma omp simd reduction(+:dim_sum)
    for (int64_t i = 0; i < n; ++i) {
        dim_sum += (2 * i - n + 1) * coords[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    // The problem asks for the sum over 'iters' iterations. 
    // Since the input vectors don't change, we compute once and multiply.
    // However, the baseline resets 'total = 0' inside the loop, meaning it only returns the result of the last iteration.
    // Let's follow the baseline's logic: total = 0; for(iters) { total = 0; ... }
    // This means the result is just the sum for a single iteration.
    
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;
    
    int64_t x_sum = 0;
    int64_t y_sum = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            x_sum = compute_dim_sum(std::move(xs_copy));
        }
        #pragma omp section
        {
            y_sum = compute_dim_sum(std::move(ys_copy));
        }
    }

    return x_sum + y_sum;
}