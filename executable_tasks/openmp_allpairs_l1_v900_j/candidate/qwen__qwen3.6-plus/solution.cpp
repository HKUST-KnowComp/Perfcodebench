#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n <= 1) return 0;

    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    for (std::size_t k = 0; k < n; ++k) {
        int64_t c = static_cast<int64_t>(2 * k) - static_cast<int64_t>(n) + 1;
        total += static_cast<int64_t>(sx[k]) * c;
        total += static_cast<int64_t>(sy[k]) * c;
    }
    return total;
}