#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

int64_t calculate_dim_sum(std::vector<int32_t> vals) {
    size_t n = vals.size();
    if (n < 2) return 0;

    // Sort to allow O(N) calculation of pairwise absolute differences
    std::sort(vals.begin(), vals.end());

    int64_t dim_sum = 0;
    // The sum of |a_i - a_j| for all i < j is equivalent to 
    // sum_{i=0}^{n-1} (i * a_i - (n - 1 - i) * a_i)
    // which simplifies to sum_{i=0}^{n-1} (2*i - n + 1) * a_i
    #pragma omp parallel for reduction(+:dim_sum)
    for (size_t i = 0; i < n; ++i) {
        dim_sum += static_cast<int64_t>(2 * (int64_t)i - (int64_t)n + 1) * vals[i];
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    // The total sum is the sum of pairwise x-distances + sum of pairwise y-distances.
    // Since the input vectors are constant across iterations, we compute the sum once.
    
    // We must copy the vectors because calculate_dim_sum sorts them in-place.
    std::vector<int32_t> x_copy = xs;
    std::vector<int32_t> y_copy = ys;

    int64_t single_iter_sum = 0;
    
    // Parallelize the two dimension calculations
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            single_iter_sum += calculate_dim_sum(x_copy);
        }
        #pragma omp section
        {
            single_iter_sum += calculate_dim_sum(y_copy);
        }
    }

    return single_iter_sum * iters;
}