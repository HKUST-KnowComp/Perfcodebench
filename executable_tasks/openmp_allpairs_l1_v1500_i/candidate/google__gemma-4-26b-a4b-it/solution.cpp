#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

/**
 * Computes the sum of all pairwise absolute differences for a single dimension.
 * Sum_{i<j} |a_i - a_j| = Sum_{i=0}^{N-1} (i * a_i - (N-1-i) * a_i) after sorting.
 */
static int64_t compute_dim_sum(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;

    // Sort coordinates to allow linear time summation of absolute differences
    std::sort(coords.begin(), coords.end());

    int64_t dim_sum = 0;
    // Use OpenMP to parallelize the reduction if N is large enough
    #pragma omp parallel for reduction(+:dim_sum)
    for (size_t i = 0; i < n; ++i) {
        // The contribution of coords[i] to the total sum is:
        // (number of elements smaller than i) * coords[i] - (number of elements larger than i) * coords[i]
        // which simplifies to: i * coords[i] - (n - 1 - i) * coords[i]
        dim_sum += static_cast<int64_t>(i) * coords[i] - static_cast<int64_t>(n - 1 - i) * coords[i];
    }

    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // The problem asks for the sum over 'iters' iterations.
    // Since the input data is constant across iterations, we compute the sum once and multiply.
    
    // We must copy the vectors because compute_dim_sum sorts them in-place.
    std::vector<int32_t> x_copy = xs;
    std::vector<int32_t> y_copy = ys;

    // The total L1 sum is the sum of X-differences + sum of Y-differences.
    // We can compute these two independently.
    int64_t x_sum = compute_dim_sum(std::move(x_copy));
    int64_t y_sum = compute_dim_sum(std::move(y_copy));

    int64_t single_iter_total = x_sum + y_sum;
    
    return single_iter_total * iters;
}