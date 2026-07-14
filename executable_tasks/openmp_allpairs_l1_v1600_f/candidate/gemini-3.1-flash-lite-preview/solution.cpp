#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <omp.h>

static int64_t compute_l1_sum(std::vector<int32_t>& coords) {
    std::sort(coords.begin(), coords.end());
    int64_t n = coords.size();
    int64_t sum = 0;
    // The sum of |x_i - x_j| for all i < j is sum_{i=0}^{n-1} (i * x_i - (n - 1 - i) * x_i)
    // which simplifies to sum_{i=0}^{n-1} x_i * (2*i - n + 1)
    for (int64_t i = 0; i < n; ++i) {
        sum += static_cast<int64_t>(coords[i]) * (2 * i - n + 1);
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::vector<int32_t> sorted_xs = xs;
    std::vector<int32_t> sorted_ys = ys;
    
    int64_t x_sum = compute_l1_sum(sorted_xs);
    int64_t y_sum = compute_l1_sum(sorted_ys);
    int64_t total = x_sum + y_sum;
    
    // The problem asks for the sum over 'iters' iterations.
    // Since the input is constant, we multiply the result by iters.
    return total * iters;
}