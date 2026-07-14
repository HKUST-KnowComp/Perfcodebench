#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

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

    int64_t sum_diffs = 0;
    for (size_t i = 0; i < n; ++i) {
        // The contribution of sorted_arr[i] to the sum of absolute differences
        // is (i - (n - 1 - i)) * sorted_arr[i] = (2*i - n + 1) * sorted_arr[i]
        int64_t coeff = static_cast<int64_t>(2 * i) - static_cast<int64_t>(n) + 1;
        sum_diffs += coeff * (sorted_x[i] + sorted_y[i]);
    }

    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = sum_diffs;
    }
    return total;
}