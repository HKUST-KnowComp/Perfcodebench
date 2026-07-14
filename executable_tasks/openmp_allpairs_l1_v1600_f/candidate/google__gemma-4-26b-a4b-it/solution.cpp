#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <omp.h>

// Function to compute sum of |a_i - a_j| for all i < j in O(N log N)
static int64_t compute_dim_sum(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // Each element coords[i] is added i times and subtracted (n - 1 - i) times
        sum += static_cast<int64_t>(coords[i]) * (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i));
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    int64_t total_sum_per_iter = 0;
    
    // Pre-calculate the sum for one iteration since the input is constant
    // We use local copies to allow sorting within the dimension function
    std::vector<int32_t> xs_copy = xs;
    std::vector<int32_t> ys_copy = ys;
    
    int64_t x_sum = 0;
    int64_t y_sum = 0;

    // Parallelize the dimension calculations
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            x_sum = compute_dim_sum(xs_copy);
        }
        #pragma omp section
        {
            y_sum = compute_dim_sum(ys_copy);
        }
    }

    total_sum_per_iter = x_sum + y_sum;

    // The total sum is simply the sum per iteration multiplied by iters
    // We use int64_t to prevent overflow during the final multiplication
    return static_cast<int64_t>(iters) * total_sum_per_iter;
}