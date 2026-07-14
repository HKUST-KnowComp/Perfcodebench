#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters;
    std::size_t n = xs.size();
    if (n <= 1) return 0;

    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t n64 = static_cast<int64_t>(n);
    for (std::size_t k = 0; k < n; ++k) {
        int64_t coeff = 2LL * k - n64 + 1;
        sum_x += static_cast<int64_t>(sx[k]) * coeff;
        sum_y += static_cast<int64_t>(sy[k]) * coeff;
    }

    return sum_x + sum_y;
}