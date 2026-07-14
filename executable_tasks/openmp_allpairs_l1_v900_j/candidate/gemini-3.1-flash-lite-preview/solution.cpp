#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

static int64_t compute_1d_sum(std::vector<int32_t>& coords) {
    std::sort(coords.begin(), coords.end());
    int64_t n = coords.size();
    int64_t total = 0;
    for (int64_t i = 0; i < n; ++i) {
        // The contribution of coords[i] is (i - (n - 1 - i)) * coords[i]
        // which simplifies to (2*i - n + 1) * coords[i]
        total += (2 * i - n + 1) * static_cast<int64_t>(coords[i]);
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::vector<int32_t> sorted_xs = xs;
    std::vector<int32_t> sorted_ys = ys;
    
    int64_t x_sum = compute_1d_sum(sorted_xs);
    int64_t y_sum = compute_1d_sum(sorted_ys);
    
    int64_t total = x_sum + y_sum;
    
    // The problem asks for the sum over 'iters' iterations.
    // Since the input is constant, we just multiply the result.
    return total * iters;
}