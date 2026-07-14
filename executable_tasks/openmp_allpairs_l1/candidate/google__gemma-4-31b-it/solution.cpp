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
    // The sum of |x_i - x_j| for i < j is sum_{i=0}^{n-1} (i * x_i - (n-1-i) * x_i)
    // which simplifies to sum_{i=0}^{n-1} (2*i - n + 1) * x_i
    for (int64_t i = 0; i < n; ++i) {
        dim_sum += (2 * i - n + 1) * coords[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    int64_t sum_x = 0;
    int64_t sum_y = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            std::vector<int32_t> xs_copy = xs;
            sum_x = compute_dim_sum(std::move(xs_copy));
        }
        #pragma omp section
        {
            std::vector<int32_t> ys_copy = ys;
            sum_y = compute_dim_sum(std::move(ys_copy));
        }
    }

    return (sum_x + sum_y) * iters;
}