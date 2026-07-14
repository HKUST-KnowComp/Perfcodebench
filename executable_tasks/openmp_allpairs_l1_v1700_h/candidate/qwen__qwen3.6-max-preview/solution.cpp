#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters == 0) return 0;
    int64_t n = static_cast<int64_t>(xs.size());
    if (n < 2) return 0;

    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    for (int64_t i = 0; i < n; ++i) {
        int64_t c = 2 * i - n + 1;
        total += static_cast<int64_t>(sx[i]) * c;
        total += static_cast<int64_t>(sy[i]) * c;
    }
    return total;
}
