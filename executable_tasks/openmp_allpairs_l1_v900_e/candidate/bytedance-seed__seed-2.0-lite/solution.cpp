#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static int64_t compute_abs_sum(const std::vector<int32_t>& vals) {
    std::vector<int32_t> sorted = vals;
    std::sort(sorted.begin(), sorted.end());
    int64_t prefix_sum = 0;
    int64_t total = 0;
    for (std::size_t k = 0; k < sorted.size(); ++k) {
        total += static_cast<int64_t>(sorted[k]) * k - prefix_sum;
        prefix_sum += sorted[k];
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters == 0) return 0;
    
    int64_t sum_x, sum_y;
    #pragma omp parallel sections
    {
        #pragma omp section
        sum_x = compute_abs_sum(xs);
        #pragma omp section
        sum_y = compute_abs_sum(ys);
    }
    
    return sum_x + sum_y;
}