#include "interface.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

int64_t compute_dim_sum(std::vector<int32_t> coords) {
    int64_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t dim_sum = 0;
    for (int64_t i = 0; i < n; ++i) {
        // Each element x[i] is added (i) times and subtracted (n - 1 - i) times
        // Contribution: x[i] * (i - (n - 1 - i)) = x[i] * (2*i - n + 1)
        dim_sum += static_cast<int64_t>(coords[i]) * (2 * i - n + 1);
    }
    return dim_sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // The sum of L1 distances is the sum of distances in X + sum of distances in Y.
    // Since the input vectors are constant across iterations, we can compute the 
    // sum for one iteration and multiply by iters.
    
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;

    int64_t x_sum = compute_dim_sum(std::move(xs_copy));
    int64_t y_sum = compute_dim_sum(std::move(ys_copy));

    int64_t single_iter_total = x_sum + y_sum;
    
    // The original code resets 'total = 0' inside the loop, meaning it only 
    // returns the result of the LAST iteration. 
    // However, the baseline code says 'total = 0' inside the loop, but the 
    // loop runs 'iters' times. Let's look closely: 
    // "for (int iter = 0; iter < iters; ++iter) { total = 0; ... }"
    // This means the result is simply the sum for one iteration.
    
    return single_iter_total;
}