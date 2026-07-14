#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters;
    std::vector<int32_t> sx = xs;
    std::vector<int32_t> sy = ys;
    std::sort(sx.begin(), sx.end());
    std::sort(sy.begin(), sy.end());
    
    int64_t total = 0;
    int64_t prefix = 0;
    for (std::size_t j = 0; j < sx.size(); ++j) {
        total += static_cast<int64_t>(j) * sx[j] - prefix;
        prefix += sx[j];
    }
    prefix = 0;
    for (std::size_t j = 0; j < sy.size(); ++j) {
        total += static_cast<int64_t>(j) * sy[j] - prefix;
        prefix += sy[j];
    }
    return total;
}
