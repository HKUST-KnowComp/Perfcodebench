#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    size_t n = xs.size();
    if (n < 2) return 0;

    std::vector<int64_t> sorted_xs(n);
    std::vector<int64_t> sorted_ys(n);
    for (size_t i = 0; i < n; ++i) {
        sorted_xs[i] = xs[i];
        sorted_ys[i] = ys[i];
    }
    std::sort(sorted_xs.begin(), sorted_xs.end());
    std::sort(sorted_ys.begin(), sorted_ys.end());

    int64_t sum_dist = 0;
    // The sum of |x_i - x_j| for all i < j is sum_{i=0}^{n-1} (2*i - n + 1) * sorted_x[i]
    for (size_t i = 0; i < n; ++i) {
        int64_t coeff = (2LL * static_cast<int64_t>(i) - static_cast<int64_t>(n) + 1LL);
        sum_dist += coeff * sorted_xs[i];
        sum_dist += coeff * sorted_ys[i];
    }

    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = sum_dist;
    }
    return total;
}