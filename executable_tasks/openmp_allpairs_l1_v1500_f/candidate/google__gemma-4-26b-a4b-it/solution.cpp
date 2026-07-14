#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

/**
 * Computes the sum of all pairwise absolute differences for a single dimension.
 * Formula: sum_{i < j} |x_i - x_j| = sum_{i=0}^{N-1} (i * x_i - (N - 1 - i) * x_i) after sorting.
 */
static int64_t compute_dim_sum(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // Each element coords[i] is added i times and subtracted (n - 1 - i) times
        sum += static_cast<int64_t>(i) * coords[i] - static_cast<int64_t>(n - 1 - i) * coords[i];
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;

    // The sum of L1 distances is the sum of X-distances + sum of Y-distances.
    // Since the points are static across iterations, we compute the sum once.
    
    // We use local copies to sort without modifying the input vectors.
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;

    int64_t x_sum = compute_dim_sum(xs_copy);
    int64_t y_sum = compute_dim_sum(ys_copy);
    
    int64_t single_iter_total = x_sum + y_sum;
    
    // The task asks for the total sum over 'iters' iterations.
    return static_cast<int64_t>(iters) * single_iter_total;
}