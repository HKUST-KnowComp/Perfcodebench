#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty() || iters <= 0) return 0;
    
    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t n = static_cast<int64_t>(sx.size());
    
    #pragma omp parallel for reduction(+:sum_x, sum_y)
    for (int64_t i = 0; i < n; ++i) {
        int64_t coeff = 2 * i - n + 1;
        sum_x += static_cast<int64_t>(sx[i]) * coeff;
        sum_y += static_cast<int64_t>(sy[i]) * coeff;
    }
    
    return (sum_x + sum_y) * iters;
}