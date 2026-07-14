#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {
int64_t compute_abs_sum(std::vector<int32_t> arr) {
    const std::size_t n = arr.size();
    if (n < 2) return 0;
    std::sort(arr.begin(), arr.end());
    int64_t prefix = 0;
    int64_t total = 0;
    for (std::size_t k = 0; k < n; ++k) {
        if (k > 0) {
            total += static_cast<int64_t>(arr[k]) * k - prefix;
        }
        prefix += arr[k];
    }
    return total;
}
} // anonymous namespace

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t x_sum = 0, y_sum = 0;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                x_sum = compute_abs_sum(xs);
            }
            #pragma omp section
            {
                y_sum = compute_abs_sum(ys);
            }
        }
        total = x_sum + y_sum;
    }
    return total;
}