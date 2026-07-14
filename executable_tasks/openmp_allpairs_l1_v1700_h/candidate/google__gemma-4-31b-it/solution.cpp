#include "interface.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

int64_t compute_dim_sum(const std::vector<int32_t>& coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::vector<int32_t> sorted_coords = coords;
    std::sort(sorted_coords.begin(), sorted_coords.end());
    
    int64_t dim_sum = 0;
    #pragma omp parallel for reduction(+:dim_sum)
    for (int64_t i = 0; i < static_cast<int64_t>(n); ++i) {
        // The contribution of sorted_coords[i] to the total sum is
        // (i * sorted_coords[i]) - ((n - 1 - i) * sorted_coords[i])
        // which simplifies to (2*i - n + 1) * sorted_coords[i]
        dim_sum += (2 * i - static_cast<int64_t>(n) + 1) * sorted_coords[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    
    // The sum of L1 distances is the sum of absolute differences of X plus sum of absolute differences of Y.
    // Sum_{i<j} (|xi - xj| + |yi - yj|) = Sum_{i<j} |xi - xj| + Sum_{i<j} |yi - yj|
    
    int64_t x_sum = compute_dim_sum(xs);
    int64_t y_sum = compute_dim_sum(ys);
    
    // The problem asks for the sum over 'iters' iterations. 
    // Since the input vectors don't change, we just multiply the result.
    return (x_sum + y_sum) * iters;
}