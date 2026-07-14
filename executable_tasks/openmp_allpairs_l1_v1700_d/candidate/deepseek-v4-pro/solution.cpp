#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty() || xs.size() <= 1) return 0;

    std::vector<int32_t> sorted_xs = xs;
    std::vector<int32_t> sorted_ys = ys;
    std::sort(sorted_xs.begin(), sorted_xs.end());
    std::sort(sorted_ys.begin(), sorted_ys.end());

    const std::size_t n = sorted_xs.size();
    int64_t sum_x = 0, prefix_x = 0;
    for (std::size_t k = 0; k < n; ++k) {
        int64_t val = sorted_xs[k];
        sum_x += val * static_cast<int64_t>(k) - prefix_x;
        prefix_x += val;
    }

    int64_t sum_y = 0, prefix_y = 0;
    for (std::size_t k = 0; k < n; ++k) {
        int64_t val = sorted_ys[k];
        sum_y += val * static_cast<int64_t>(k) - prefix_y;
        prefix_y += val;
    }

    return (sum_x + sum_y) * static_cast<int64_t>(iters);
}