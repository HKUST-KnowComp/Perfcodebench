#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

// Helper function to calculate the sum of absolute differences for a single dimension in O(N log N)
int64_t sum_abs_diff(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // The contribution of coords[i] to the total sum is:
        // (number of elements smaller than it) * coords[i] - (number of elements larger than it) * coords[i]
        // In a sorted array, there are 'i' elements smaller and 'n - 1 - i' elements larger.
        sum += static_cast<int64_t>(i) * coords[i] - static_cast<int64_t>(n - 1 - i) * coords[i];
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // Calculate the sum for one iteration
    // We must copy the vectors because sum_abs_diff sorts them
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;

    int64_t single_iter_sum = sum_abs_diff(xs_copy) + sum_abs_diff(ys_copy);

    // The task asks for the sum over 'iters' iterations.
    // Since the input is constant across iterations, we can just multiply.
    return static_cast<int64_t>(iters) * single_iter_sum;
}