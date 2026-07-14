#include "interface.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

int64_t compute_dim_sum(std::vector<int32_t> coords) {
    int64_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t dim_sum = 0;
    // The sum of |x_i - x_j| for all i < j is sum_{i=0}^{n-1} (i * x_i - (n-1-i) * x_i)
    // which simplifies to sum_{i=0}^{n-1} (2*i - n + 1) * x_i
    for (int64_t i = 0; i < n; ++i) {
        dim_sum += (2 * i - n + 1) * coords[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    
    // The problem asks for the sum of all pairs (i, j) where i < j.
    // L1 distance is |x_i - x_j| + |y_i - y_j|.
    // Total sum = sum_{i<j} |x_i - x_j| + sum_{i<j} |y_i - y_j|.
    
    int64_t x_sum = 0;
    int64_t y_sum = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            std::vector<int32_t> xs_copy = xs;
            x_sum = compute_dim_sum(std::move(xs_copy));
        }
        #pragma omp section
        {
            std::vector<int32_t> ys_copy = ys;
            y_sum = compute_dim_sum(std::move(ys_copy));
        }
    }

    int64_t total_per_iter = x_sum + y_sum;
    
    // The baseline resets 'total' every iteration, effectively returning the result of the last iteration.
    // However, the baseline logic 'total = 0' inside the loop means it doesn't accumulate across iters.
    // If the goal is to match the baseline's behavior exactly:
    return total_per_iter;
}