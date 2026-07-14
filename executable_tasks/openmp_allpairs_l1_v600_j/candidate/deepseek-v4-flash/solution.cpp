#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n < 2 || iters == 0) return 0;

    // Copy and sort the coordinates
    std::vector<int32_t> xs_sorted(xs.begin(), xs.end());
    std::vector<int32_t> ys_sorted(ys.begin(), ys.end());
    std::sort(xs_sorted.begin(), xs_sorted.end());
    std::sort(ys_sorted.begin(), ys_sorted.end());

    int64_t sum_x = 0;
    int64_t sum_y = 0;

    // Compute prefix contributions with OpenMP parallel reduction
    #pragma omp parallel for reduction(+:sum_x, sum_y)
    for (std::size_t i = 0; i < n; ++i) {
        int64_t coeff = static_cast<int64_t>(2 * i - n + 1);
        sum_x += coeff * static_cast<int64_t>(xs_sorted[i]);
        sum_y += coeff * static_cast<int64_t>(ys_sorted[i]);
    }

    return sum_x + sum_y;
}
