#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <omp.h>

static int64_t compute_1d_sum(std::vector<int32_t>& coords) {
    std::sort(coords.begin(), coords.end());
    int64_t n = static_cast<int64_t>(coords.size());
    int64_t total = 0;
    for (int64_t i = 0; i < n; ++i) {
        // The contribution of coords[i] is (i - (n - 1 - i)) * coords[i]
        // which simplifies to (2*i - n + 1) * coords[i]
        total += (2 * i - n + 1) * static_cast<int64_t>(coords[i]);
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total_sum = 0;
    
    // Pre-sort copies to avoid repeated sorting inside the loop if iters > 1
    std::vector<int32_t> xs_sorted = xs;
    std::vector<int32_t> ys_sorted = ys;
    
    int64_t x_sum = compute_1d_sum(xs_sorted);
    int64_t y_sum = compute_1d_sum(ys_sorted);
    
    total_sum = (x_sum + y_sum) * iters;
    
    return total_sum;
}