#include "interface.h"
#include <omp.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

static int64_t calculate_abs_sum(std::vector<int32_t> arr) {
    const std::size_t n = arr.size();
    if (n < 2) return 0;
    std::sort(arr.begin(), arr.end());
    std::vector<int64_t> prefix(n, 0);
    for (std::size_t i = 1; i < n; ++i) {
        prefix[i] = prefix[i-1] + arr[i-1];
    }
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (std::size_t k = 0; k < n; ++k) {
        total += static_cast<int64_t>(arr[k]) * k - prefix[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const int64_t sum_x = calculate_abs_sum(xs);
        const int64_t sum_y = calculate_abs_sum(ys);
        total = sum_x + sum_y;
    }
    return total;
}