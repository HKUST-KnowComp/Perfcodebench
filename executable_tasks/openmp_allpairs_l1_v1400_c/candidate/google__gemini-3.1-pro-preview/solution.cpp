#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

/**
 * Computes the sum of all pairwise L1 distances: Σ_{i<j} (|x_i - x_j| + |y_i - y_j|).
 * This can be decomposed into Σ_{i<j} |x_i - x_j| + Σ_{i<j} |y_i - y_j|.
 * For a sorted sequence z_0 ≤ z_1 ≤ ... ≤ z_{n-1}, the sum of absolute differences is:
 * Σ_{i=0}^{n-1} z_i * (2*i - n + 1).
 */
static int64_t compute_dim_sum(const std::vector<int32_t>& vec) {
    size_t n = vec.size();
    if (n < 2) return 0;

    std::vector<int32_t> sorted_vec = vec;
    std::sort(sorted_vec.begin(), sorted_vec.end());

    int64_t dim_total = 0;
    // Use OpenMP to parallelize the linear summation if n is large
    #pragma omp parallel reduction(+:dim_total)
    {
        int num_threads = omp_get_num_threads();
        int thread_id = omp_get_thread_num();
        size_t chunk_size = (n + num_threads - 1) / num_threads;
        size_t start = thread_id * chunk_size;
        size_t end = std::min(start + chunk_size, n);

        int64_t local_sum = 0;
        for (size_t i = start; i < end; ++i) {
            // Formula: z_i * (2*i - (n - 1))
            local_sum += static_cast<int64_t>(sorted_vec[i]) * (2 * static_cast<int64_t>(i) - static_cast<int64_t>(n) + 1);
        }
        dim_total = local_sum;
    }

    return dim_total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int64_t sum_x = 0;
        int64_t sum_y = 0;

        // Process dimensions in parallel
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                sum_x = compute_dim_sum(xs);
            }
            #pragma omp section
            {
                sum_y = compute_dim_sum(ys);
            }
        }

        total = sum_x + sum_y;
    }
    return total;
}