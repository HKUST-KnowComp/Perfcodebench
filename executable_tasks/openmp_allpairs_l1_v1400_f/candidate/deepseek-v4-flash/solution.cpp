#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
    std::size_t n = xs.size();
    if (n == 0) return 0;

    std::vector<int32_t> xs_sorted(xs.begin(), xs.end());
    std::vector<int32_t> ys_sorted(ys.begin(), ys.end());
    std::sort(xs_sorted.begin(), xs_sorted.end());
    std::sort(ys_sorted.begin(), ys_sorted.end());

    int64_t total_x = 0;
    int64_t total_y = 0;
    for (std::size_t i = 0; i < n; ++i) {
        int64_t mul = static_cast<int64_t>(2 * i) - static_cast<int64_t>(n) + 1;
        total_x += static_cast<int64_t>(xs_sorted[i]) * mul;
        total_y += static_cast<int64_t>(ys_sorted[i]) * mul;
    }
    return total_x + total_y;
}