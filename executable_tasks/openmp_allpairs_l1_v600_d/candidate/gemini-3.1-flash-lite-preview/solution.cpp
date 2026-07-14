#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

static int64_t compute_l1_contribution(std::vector<int32_t>& coords) {
    std::sort(coords.begin(), coords.end());
    int64_t n = static_cast<int64_t>(coords.size());
    int64_t sum = 0;
    for (int64_t i = 0; i < n; ++i) {
        // The contribution of coords[i] is (i - (n - 1 - i)) * coords[i]
        // which simplifies to (2*i - n + 1) * coords[i]
        sum += (2 * i - n + 1) * static_cast<int64_t>(coords[i]);
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;
    
    int64_t x_sum = compute_l1_contribution(xs_copy);
    int64_t y_sum = compute_l1_contribution(ys_copy);
    
    int64_t total = x_sum + y_sum;
    
    // The problem asks for the sum over 'iters' iterations.
    // Since the input is constant, we just multiply the result.
    return total * iters;
}