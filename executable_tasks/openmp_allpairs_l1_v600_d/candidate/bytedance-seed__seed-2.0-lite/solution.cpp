#include "interface.h"
#include <omp.h>
#include <vector>
#include <algorithm>
#include <cstdint>

static int64_t compute_1d_abs_sum(const std::vector<int32_t>& arr) {
    const std::size_t n = arr.size();
    if (n <= 1) return 0;
    std::vector<int32_t> tmp = arr;
    std::sort(tmp.begin(), tmp.end());
    int64_t prefix_sum = 0;
    int64_t total = 0;
    #pragma omp simd reduction(+:total, prefix_sum)
    for (std::size_t k = 0; k < n; ++k) {
        total += static_cast<int64_t>(tmp[k]) * k - prefix_sum;
        prefix_sum += tmp[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t x_sum, y_sum;
        #pragma omp parallel sections
        {
            #pragma omp section
            x_sum = compute_1d_abs_sum(xs);
            #pragma omp section
            y_sum = compute_1d_abs_sum(ys);
        }
        total = x_sum + y_sum;
    }
    return total;
}