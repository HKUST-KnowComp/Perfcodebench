#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters;
    if (xs.empty()) return 0;

    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    int64_t n = static_cast<int64_t>(sx.size());
    for (int64_t k = 0; k < n; ++k) {
        int64_t coeff = 2 * k - n + 1;
        total += static_cast<int64_t>(sx[k]) * coeff;
        total += static_cast<int64_t>(sy[k]) * coeff;
    }
    return total;
}
