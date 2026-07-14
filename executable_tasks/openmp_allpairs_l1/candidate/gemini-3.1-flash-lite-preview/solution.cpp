#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    size_t n = xs.size();
    if (n < 2) return 0;

    std::vector<int64_t> sorted_x(n);
    std::vector<int64_t> sorted_y(n);
    for (size_t i = 0; i < n; ++i) {
        sorted_x[i] = xs[i];
        sorted_y[i] = ys[i];
    }
    std::sort(sorted_x.begin(), sorted_x.end());
    std::sort(sorted_y.begin(), sorted_y.end());

    int64_t sum_1d = 0;
    for (size_t i = 0; i < n; ++i) {
        // The contribution of sorted_x[i] is (i - (n - 1 - i)) * sorted_x[i]
        sum_1d += (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i)) * sorted_x[i];
        sum_1d += (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i)) * sorted_y[i];
    }

    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (int iter = 0; iter < iters; ++iter) {
        total += sum_1d;
    }
    return total;
}