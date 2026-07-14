#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.size() <= 1) return 0;
    
    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t n = xs.size();
    
    for (int64_t k = 0; k < n; ++k) {
        sum_x += (2 * k - n + 1) * static_cast<int64_t>(sx[k]);
        sum_y += (2 * k - n + 1) * static_cast<int64_t>(sy[k]);
    }
    
    return sum_x + sum_y;
}
