#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    size_t n = xs.size();
    std::vector<int32_t> sorted_x = xs;
    std::vector<int32_t> sorted_y = ys;

    std::sort(sorted_x.begin(), sorted_x.end());
    std::sort(sorted_y.begin(), sorted_y.end());

    int64_t x_sum = 0;
    int64_t y_sum = 0;

    // The sum of |a_i - a_j| for all i < j is sum_{i=0}^{n-1} (2*i - n + 1) * a_i
    // where a is sorted.
    for (int64_t i = 0; i < static_cast<int64_t>(n); ++i) {
        int64_t weight = 2 * i - static_cast<int64_t>(n) + 1;
        x_sum += weight * sorted_x[i];
        y_sum += weight * sorted_y[i];
    }

    return (x_sum + y_sum) * iters;
}