#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int n = xs.size();
    if (n == 0) return 0;

    // Create sorted copies once for all iterations
    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum = 0;
        // Sum over all pairs of |x_i - x_j| using sorted formula
        for (int i = 0; i < n; ++i) {
            sum += static_cast<int64_t>(sx[i]) * (2LL * i - n + 1);
        }
        // Sum over all pairs of |y_i - y_j|
        for (int i = 0; i < n; ++i) {
            sum += static_cast<int64_t>(sy[i]) * (2LL * i - n + 1);
        }
        total = sum;
    }
    return total;
}
