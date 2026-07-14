#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters == 0) return 0;

    auto compute_dim = [](const std::vector<int32_t>& v) -> int64_t {
        std::vector<int32_t> sorted_v = v;
        std::sort(sorted_v.begin(), sorted_v.end());
        int64_t sum = 0;
        int64_t n = sorted_v.size();
        for (int64_t k = 0; k < n; ++k) {
            sum += static_cast<int64_t>(sorted_v[k]) * (2 * k - n + 1);
        }
        return sum;
    };

    return compute_dim(xs) + compute_dim(ys);
}
