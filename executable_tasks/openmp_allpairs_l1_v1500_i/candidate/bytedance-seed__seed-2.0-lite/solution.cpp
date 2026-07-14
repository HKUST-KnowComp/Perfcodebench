#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static int64_t calculate_pairwise_abs_sum(std::vector<int32_t> vals) {
    std::sort(vals.begin(), vals.end());
    int64_t prefix_sum = 0;
    int64_t total = 0;
    for (size_t k = 0; k < vals.size(); ++k) {
        total += static_cast<int64_t>(vals[k]) * k - prefix_sum;
        prefix_sum += vals[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    int64_t sum_x, sum_y;

    #pragma omp parallel sections
    {
        #pragma omp section
        sum_x = calculate_pairwise_abs_sum(xs);
        #pragma omp section
        sum_y = calculate_pairwise_abs_sum(ys);
    }

    const int64_t total_sum = sum_x + sum_y;
    for (int iter = 0; iter < iters; ++iter) {
        total = total_sum;
    }
    return total;
}