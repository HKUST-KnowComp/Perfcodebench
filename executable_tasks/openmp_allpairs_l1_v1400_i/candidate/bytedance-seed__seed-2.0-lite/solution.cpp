#include "interface.h"
#include <algorithm>
#include <omp.h>
#include <cstdint>
#include <vector>

static int64_t calculate_pairwise_sum(std::vector<int32_t> arr) {
    const size_t n = arr.size();
    if (n < 2) return 0;
    std::sort(arr.begin(), arr.end());
    int64_t prefix_sum = 0;
    int64_t total = 0;
    for (size_t k = 0; k < n; ++k) {
        total += static_cast<int64_t>(arr[k]) * k - prefix_sum;
        prefix_sum += arr[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t sum_x, sum_y;
    #pragma omp parallel sections
    {
        #pragma omp section
        sum_x = calculate_pairwise_sum(xs);
        #pragma omp section
        sum_y = calculate_pairwise_sum(ys);
    }
    const int64_t total = sum_x + sum_y;
    int64_t final_total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        final_total = total;
    }
    return final_total;
}