#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

// Function to compute the sum of absolute differences for a single dimension in O(N log N)
int64_t compute_dim_sum(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;

    // Sort coordinates to allow linear time summation of absolute differences
    std::sort(coords.begin(), coords.end());

    int64_t sum = 0;
    // The sum of |a_i - a_j| for all i < j is equivalent to 
    // sum_{i=0}^{n-1} [ i * a_i - (n - 1 - i) * a_i ]
    // We use a parallel reduction for the summation part
    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < n; ++i) {
        sum += static_cast<int64_t>(i) * coords[i] - static_cast<int64_t>(n - 1 - i) * coords[i];
    }

    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // Pre-calculate the sum for one iteration
    // We must copy the vectors because compute_dim_sum sorts them
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;

    int64_t single_iter_sum = compute_dim_sum(xs_copy) + compute_dim_sum(ys_copy);

    // The task asks for the sum over 'iters' iterations
    return static_cast<int64_t>(iters) * single_iter_sum;
}