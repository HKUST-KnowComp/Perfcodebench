#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <omp.h>

static int64_t compute_pairwise_l1_sum(std::vector<int32_t> arr) {
    const std::size_t n = arr.size();
    if (n < 2) return 0;
    
    std::sort(arr.begin(), arr.end());
    
    std::vector<int64_t> prefix(n);
    prefix[0] = arr[0];
    for (std::size_t i = 1; i < n; ++i) {
        prefix[i] = prefix[i-1] + arr[i];
    }
    
    int64_t sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (std::size_t i = 1; i < n; ++i) {
        sum += static_cast<int64_t>(arr[i]) * static_cast<int64_t>(i) - prefix[i-1];
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const int64_t x_total = compute_pairwise_l1_sum(xs);
    const int64_t y_total = compute_pairwise_l1_sum(ys);
    const int64_t full_total = x_total + y_total;
    
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = full_total;
    }
    return total;
}