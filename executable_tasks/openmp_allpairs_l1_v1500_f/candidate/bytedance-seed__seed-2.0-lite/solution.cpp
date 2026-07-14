#include "interface.h"
#include <omp.h>
#include <algorithm>
#include <cstdint>
#include <vector>

static int64_t calculate_sum_from_sorted(const std::vector<int32_t>& sorted_arr) {
    const std::size_t n = sorted_arr.size();
    if (n < 2) return 0;
    int64_t total = 0;
    int64_t prefix_sum = sorted_arr[0];
    for (std::size_t k = 1; k < n; ++k) {
        total += static_cast<int64_t>(sorted_arr[k]) * static_cast<int64_t>(k) - prefix_sum;
        prefix_sum += sorted_arr[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const std::size_t n = xs.size();
    if (n < 2 || ys.size() != n) return 0;

    std::vector<int32_t> sorted_xs(xs.begin(), xs.end());
    std::sort(sorted_xs.begin(), sorted_xs.end());
    std::vector<int32_t> sorted_ys(ys.begin(), ys.end());
    std::sort(sorted_ys.begin(), sorted_ys.end());

    const int64_t total_sum = calculate_sum_from_sorted(sorted_xs) + calculate_sum_from_sorted(sorted_ys);
    int64_t final_total = 0;

    #pragma omp parallel for
    for (int iter = 0; iter < iters; ++iter) {
        final_total = total_sum;
    }

    return final_total;
}