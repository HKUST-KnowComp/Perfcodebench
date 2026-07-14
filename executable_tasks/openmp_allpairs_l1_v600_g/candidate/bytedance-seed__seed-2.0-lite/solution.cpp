#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

namespace {
int64_t compute_pairwise_abs_sum(std::vector<int32_t> arr) {
    const std::size_t n = arr.size();
    if (n <= 1) return 0;
    std::sort(arr.begin(), arr.end());
    std::vector<int64_t> prefix(n + 1, 0);
    for (std::size_t i = 0; i < n; ++i) {
        prefix[i+1] = prefix[i] + arr[i];
    }
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (std::size_t i = 0; i < n; ++i) {
        total += static_cast<int64_t>(arr[i]) * i - prefix[i];
    }
    return total;
}
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const int64_t sum_x = compute_pairwise_abs_sum(xs);
        const int64_t sum_y = compute_pairwise_abs_sum(ys);
        total = sum_x + sum_y;
    }
    return total;
}