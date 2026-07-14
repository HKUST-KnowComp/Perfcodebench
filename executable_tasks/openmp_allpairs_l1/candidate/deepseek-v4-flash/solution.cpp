#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    if (n < 2) return 0;

    // Compute sum of |x_i - x_j| for i<j using sorting and prefix sums
    auto compute_sum = [](const std::vector<int32_t>& vals) -> __int128 {
        std::vector<int32_t> sorted(vals.begin(), vals.end());
        std::sort(sorted.begin(), sorted.end());
        __int128 prefix = 0;
        __int128 total = 0;
        for (std::size_t j = 0; j < sorted.size(); ++j) {
            __int128 val = sorted[j];
            total += val * static_cast<__int128>(j) - prefix;
            prefix += val;
        }
        return total;
    };

    __int128 sum_x = compute_sum(xs);
    __int128 sum_y = compute_sum(ys);
    __int128 total = (sum_x + sum_y) * static_cast<__int128>(iters);
    return static_cast<int64_t>(total);
}
