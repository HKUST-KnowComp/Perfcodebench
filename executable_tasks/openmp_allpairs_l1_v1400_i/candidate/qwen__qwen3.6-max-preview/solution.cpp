#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n < 2) return 0;

    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());

    int64_t total = 0;
    int64_t n64 = static_cast<int64_t>(n);
    for (int64_t k = 0; k < n64; ++k) {
        int64_t coeff = 2 * k - n64 + 1;
        total += static_cast<int64_t>(sx[k]) * coeff;
        total += static_cast<int64_t>(sy[k]) * coeff;
    }
    
    (void)iters;
    return total;
}
