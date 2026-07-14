#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters;
    size_t n = xs.size();
    if (n <= 1) return 0;
    
    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t total = 0;
    for (size_t i = 0; i < n; ++i) {
        int64_t coeff = (int64_t)(2 * i) - (int64_t)n + 1;
        total += coeff * ((int64_t)sx[i] + (int64_t)sy[i]);
    }
    return total;
}