#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n < 2) return 0;
    
    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t coeff = 1 - static_cast<int64_t>(n);
    
    for (std::size_t i = 0; i < n; ++i) {
        sum_x += static_cast<int64_t>(sx[i]) * coeff;
        sum_y += static_cast<int64_t>(sy[i]) * coeff;
        coeff += 2;
    }
    
    return sum_x + sum_y;
}