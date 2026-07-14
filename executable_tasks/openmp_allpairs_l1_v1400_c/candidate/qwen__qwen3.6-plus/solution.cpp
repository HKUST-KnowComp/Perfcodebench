#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters;
    std::size_t n = xs.size();
    if (n <= 1) return 0;

    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t sum = 0;
    int64_t n_ll = static_cast<int64_t>(n);
    for (std::size_t k = 0; k < n; ++k) {
        int64_t coeff = 2LL * static_cast<int64_t>(k) - n_ll + 1;
        sum += static_cast<int64_t>(sx[k]) * coeff;
        sum += static_cast<int64_t>(sy[k]) * coeff;
    }

    return sum;
}
