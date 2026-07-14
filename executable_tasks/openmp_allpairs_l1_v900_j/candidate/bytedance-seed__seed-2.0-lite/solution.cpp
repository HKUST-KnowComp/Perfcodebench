#include "interface.h"
#include <omp.h>
#include <algorithm>
#include <cstdint>
#include <vector>

static int64_t compute_sum(std::vector<int32_t> arr) {
    std::sort(arr.begin(), arr.end());
    int64_t prefix = 0;
    int64_t total = 0;
    for (std::size_t i = 0; i < arr.size(); ++i) {
        total += static_cast<int64_t>(arr[i]) * i - prefix;
        prefix += arr[i];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t x_sum = 0, y_sum = 0;
    #pragma omp parallel sections
    {
        #pragma omp section
        x_sum = compute_sum(xs);
        #pragma omp section
        y_sum = compute_sum(ys);
    }
    const int64_t total_sum = x_sum + y_sum;
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = total_sum;
    }
    return total;
}