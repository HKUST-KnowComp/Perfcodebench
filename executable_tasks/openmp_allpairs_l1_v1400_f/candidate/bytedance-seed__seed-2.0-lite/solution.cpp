#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static int64_t compute_pairwise_l1_sum(const std::vector<int32_t>& arr) {
    const size_t n = arr.size();
    if (n < 2) return 0;

    std::vector<int32_t> sorted = arr;
    std::sort(sorted.begin(), sorted.end());

    std::vector<int64_t> prefix(n);
    prefix[0] = sorted[0];
    for (size_t i = 1; i < n; ++i) {
        prefix[i] = prefix[i-1] + sorted[i];
    }

    int64_t total = 0;
    #pragma omp parallel for reduction(+:total)
    for (size_t k = 1; k < n; ++k) {
        total += static_cast<int64_t>(sorted[k]) * k - prefix[k-1];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        const int64_t sum_x = compute_pairwise_l1_sum(xs);
        const int64_t sum_y = compute_pairwise_l1_sum(ys);
        total = sum_x + sum_y;
    }
    return total;
}