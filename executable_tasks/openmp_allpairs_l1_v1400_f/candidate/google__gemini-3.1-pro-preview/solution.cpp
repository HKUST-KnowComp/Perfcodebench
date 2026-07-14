#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

/**
 * Computes the sum of all pairwise L1 distances: sum_{i < j} (|xs[i] - xs[j]| + |ys[i] - ys[j]|).
 * The L1 distance sum is separable: sum_{i < j} |xs[i] - xs[j]| + sum_{i < j} |ys[i] - ys[j]|.
 * For a sorted sequence a_0 <= a_1 <= ... <= a_{N-1}, the sum of absolute differences is:
 * sum_{i=0}^{N-1} a_i * (2*i - N + 1).
 */
static int64_t compute_sorted_l1_sum(std::vector<int32_t>& vals) {
    size_t n = vals.size();
    if (n < 2) return 0;

    // Sort the values to eliminate the absolute value operator
    std::sort(vals.begin(), vals.end());

    int64_t sum = 0;
    // Parallelize the linear summation using OpenMP reduction
    #pragma omp parallel for reduction(+:sum)
    for (int64_t i = 0; i < static_cast<int64_t>(n); ++i) {
        // Coefficient is (i) - (n - 1 - i) = 2*i - n + 1
        sum += static_cast<int64_t>(vals[i]) * (2 * i - static_cast<int64_t>(n) + 1);
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    int64_t total = 0;
    // The result is the same for every iteration as inputs are constant.
    // We perform the calculation inside the loop to satisfy the 'iters' requirement,
    // though in a real-world scenario we would cache the result.
    for (int iter = 0; iter < iters; ++iter) {
        std::vector<int32_t> x_coords = xs;
        std::vector<int32_t> y_coords = ys;

        int64_t x_sum = 0;
        int64_t y_sum = 0;

        // Use sections to potentially sort x and y in parallel if threads are available
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                x_sum = compute_sorted_l1_sum(x_coords);
            }
            #pragma omp section
            {
                y_sum = compute_sorted_l1_sum(y_coords);
            }
        }

        total = x_sum + y_sum;
    }

    return total;
}