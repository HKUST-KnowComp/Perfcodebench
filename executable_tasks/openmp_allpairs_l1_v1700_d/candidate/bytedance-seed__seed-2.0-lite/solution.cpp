#include "interface.h"
#include <omp.h>
#include <cstdint>
#include <vector>
#include <algorithm>

static int64_t compute_pairwise_sum(std::vector<int32_t> arr) {
    const std::size_t n = arr.size();
    if (n < 2) return 0;
    std::sort(arr.begin(), arr.end());
    int64_t prefix_sum = 0;
    int64_t total = 0;
    for (std::size_t i = 0; i < n; ++i) {
        total += static_cast<int64_t>(arr[i]) * i - prefix_sum;
        prefix_sum += arr[i];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum_x = 0, sum_y = 0;
        #pragma omp parallel sections
        {
            #pragma omp section
            sum_x = compute_pairwise_sum(xs);
            #pragma omp section
            sum_y = compute_pairwise_sum(ys);
        }
        total = sum_x + sum_y;
    }
    return total;
}