#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    std::size_t n = xs.size();
    if (n == 0) return 0;

    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        int64_t w = 2LL * i - static_cast<int64_t>(n) + 1;
        sum += static_cast<int64_t>(sx[i]) * w + static_cast<int64_t>(sy[i]) * w;
    }
    return sum;
}