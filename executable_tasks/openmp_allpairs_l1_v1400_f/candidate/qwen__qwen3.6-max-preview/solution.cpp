#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n < 2) return 0;

    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    int64_t N = static_cast<int64_t>(n);
    for (std::size_t i = 0; i < n; ++i) {
        int64_t coeff = 2LL * static_cast<int64_t>(i) - N + 1;
        total += static_cast<int64_t>(sx[i]) * coeff;
        total += static_cast<int64_t>(sy[i]) * coeff;
    }

    return total * iters;
}
