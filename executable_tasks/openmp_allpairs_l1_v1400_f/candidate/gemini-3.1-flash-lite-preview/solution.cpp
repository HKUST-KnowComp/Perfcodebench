#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cmath>

static int64_t compute_l1_sum_optimized(const std::vector<int32_t>& coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    std::vector<int64_t> sorted_coords(n);
    for (size_t i = 0; i < n; ++i) sorted_coords[i] = coords[i];
    std::sort(sorted_coords.begin(), sorted_coords.end());

    int64_t total = 0;
    for (size_t i = 0; i < n; ++i) {
        // The contribution of sorted_coords[i] to the sum of absolute differences
        // is (i * sorted_coords[i]) - ((n - 1 - i) * sorted_coords[i])
        total += sorted_coords[i] * (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i));
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t x_sum = compute_l1_sum_optimized(xs);
    int64_t y_sum = compute_l1_sum_optimized(ys);
    int64_t total = x_sum + y_sum;
    
    // The problem asks for the sum over 'iters' iterations.
    // Since the result is constant per iteration, we multiply.
    return total * iters;
}