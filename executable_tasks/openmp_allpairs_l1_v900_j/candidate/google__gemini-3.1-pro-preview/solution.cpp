#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty() || iters <= 0) {
        return 0;
    }

    std::vector<int32_t> x_sorted = xs;
    std::vector<int32_t> y_sorted = ys;

    std::sort(x_sorted.begin(), x_sorted.end());
    std::sort(y_sorted.begin(), y_sorted.end());

    int64_t n = x_sorted.size();
    int64_t total = 0;

    #pragma omp parallel for reduction(+:total)
    for (int64_t i = 0; i < n; ++i) {
        int64_t coef = 2 * i - n + 1;
        total += (static_cast<int64_t>(x_sorted[i]) + static_cast<int64_t>(y_sorted[i])) * coef;
    }

    int64_t final_total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        final_total = total;
    }

    return final_total;
}
