#include "interface.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

/**
 * The sum of all pairwise L1 distances is the sum of pairwise distances in X plus the sum of pairwise distances in Y.
 * For a sorted array A of size N, the sum of |A[i] - A[j]| for all i < j is:
 * Sum = sum_{i=0}^{N-1} (i * A[i] - (N - 1 - i) * A[i])
 *     = sum_{i=0}^{N-1} (2*i - N + 1) * A[i]
 */
int64_t compute_dim_sum(std::vector<int32_t> coords) {
    int64_t n = static_cast<int64_t>(coords.size());
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t dim_sum = 0;
    for (int64_t i = 0; i < n; ++i) {
        dim_sum += (2 * i - n + 1) * coords[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    
    // We only need to compute the sum once and multiply by iters, 
    // as the input vectors do not change between iterations.
    int64_t x_sum = 0;
    int64_t y_sum = 0;

    // Use OpenMP to compute X and Y dimensions in parallel
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            x_sum = compute_dim_sum(xs);
        }
        #pragma omp section
        {
            y_sum = compute_dim_sum(ys);
        }
    }

    return (x_sum + y_sum) * iters;
}