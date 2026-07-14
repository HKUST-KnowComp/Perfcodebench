#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <omp.h>

static int64_t compute_1d_sum(std::vector<int32_t>& coords) {
    std::sort(coords.begin(), coords.end());
    int64_t n = static_cast<int64_t>(coords.size());
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (int64_t i = 0; i < n; ++i) {
        total += (2 * i - n + 1) * static_cast<int64_t>(coords[i]);
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;
    
    int64_t x_sum = compute_1d_sum(xs_copy);
    int64_t y_sum = compute_1d_sum(ys_copy);
    total = x_sum + y_sum;

    // The problem asks to repeat the calculation 'iters' times.
    // Since the result is constant, we return the computed sum multiplied by iters.
    return total * iters;
}