#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static int64_t compute_1d_sum(const std::vector<int32_t>& arr) {
    const size_t n = arr.size();
    if (n < 2) return 0;
    std::vector<int32_t> sorted_arr = arr;
    std::sort(sorted_arr.begin(), sorted_arr.end());
    
    std::vector<int64_t> prefix(n + 1, 0);
    for (size_t i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + sorted_arr[i];
    }
    
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (size_t i = 0; i < n; ++i) {
        total += static_cast<int64_t>(sorted_arr[i]) * i - prefix[i];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        const int64_t sum_x = compute_1d_sum(xs);
        const int64_t sum_y = compute_1d_sum(ys);
        total = sum_x + sum_y;
    }
    return total;
}