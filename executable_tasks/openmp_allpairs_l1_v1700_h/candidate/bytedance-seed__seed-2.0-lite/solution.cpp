#include "interface.h"
#include <omp.h>
#include <algorithm>
#include <numeric>
#include <cstdint>
#include <vector>

static int64_t calculate_pairwise_abs_sum(std::vector<int32_t> data) {
    const std::size_t n = data.size();
    if (n < 2) return 0;
    std::sort(data.begin(), data.end());
    std::vector<int64_t> prefix(n);
    std::partial_sum(data.begin(), data.end(), prefix.begin());
    int64_t total = 0;
    for (std::size_t i = 1; i < n; ++i) {
        total += static_cast<int64_t>(data[i]) * i - prefix[i-1];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t x_sum = 0, y_sum = 0;
    #pragma omp parallel sections
    {
        #pragma omp section
        x_sum = calculate_pairwise_abs_sum(xs);
        #pragma omp section
        y_sum = calculate_pairwise_abs_sum(ys);
    }
    const int64_t per_iter_total = x_sum + y_sum;
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = per_iter_total;
    }
    return total;
}