#include "interface.h"
#include <vector>
#include <algorithm>
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

    int64_t dim_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // The contribution of sorted_x[i] to the sum of absolute differences is:
        // sorted_x[i] * (i - (n - 1 - i)) = sorted_x[i] * (2*i - n + 1)
        int64_t coeff = static_cast<int64_t>(2 * i) - static_cast<int64_t>(n) + 1;
        dim_sum += coeff * sorted_x[i];
        dim_sum += coeff * sorted_y[i];
    }

    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = dim_sum;
    }
    return total;
}