#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0 || xs.empty()) return 0;
    
    std::size_t n = xs.size();
    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t sum = 0;
    for (std::size_t k = 0; k < n; ++k) {
        int64_t coef = 2LL * (int64_t)k - (int64_t)n + 1;
        sum += (int64_t)sx[k] * coef;
        sum += (int64_t)sy[k] * coef;
    }
    
    return sum * (int64_t)iters;
}