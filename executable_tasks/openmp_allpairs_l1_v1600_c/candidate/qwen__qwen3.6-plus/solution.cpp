#include "interface.h"
#include <algorithm>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    size_t n = xs.size();
    if (n <= 1) return 0;
    
    std::vector<int32_t> sx(xs);
    std::vector<int32_t> sy(ys);
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t sum_x = 0;
    int64_t sum_y = 0;
    int64_t prefix_x = 0;
    int64_t prefix_y = 0;
    
    for (size_t k = 0; k < n; ++k) {
        sum_x += static_cast<int64_t>(sx[k]) * k - prefix_x;
        prefix_x += sx[k];
        
        sum_y += static_cast<int64_t>(sy[k]) * k - prefix_y;
        prefix_y += sy[k];
    }
    
    return (sum_x + sum_y) * iters;
}
