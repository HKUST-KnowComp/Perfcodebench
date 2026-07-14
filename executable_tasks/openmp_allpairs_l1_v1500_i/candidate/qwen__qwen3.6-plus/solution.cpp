#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

static int64_t sum_abs_diff(std::vector<int32_t> v) {
    std::sort(v.begin(), v.end());
    int64_t sum = 0;
    int64_t n = static_cast<int64_t>(v.size());
    for (int64_t i = 0; i < n; ++i) {
        sum += static_cast<int64_t>(v[i]) * (2 * i - n + 1);
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0 || xs.size() < 2) return 0;
    int64_t one_iter = sum_abs_diff(xs) + sum_abs_diff(ys);
    return one_iter * static_cast<int64_t>(iters);
}