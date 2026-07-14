#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty() || iters <= 0) return 0;

    auto compute_dim_sum = [](const std::vector<int32_t>& coords) -> int64_t {
        std::vector<int32_t> sorted_coords = coords;
        std::sort(sorted_coords.begin(), sorted_coords.end());
        
        int64_t dim_sum = 0;
        int64_t n = static_cast<int64_t>(sorted_coords.size());
        for (int64_t i = 0; i < n; ++i) {
            // Each element sorted_coords[i] is added (i) times and subtracted (n - 1 - i) times
            // Contribution: (i - (n - 1 - i)) * sorted_coords[i] = (2*i - n + 1) * sorted_coords[i]
            dim_sum += (2 * i - n + 1) * sorted_coords[i];
        }
        return dim_sum;
    };

    int64_t x_sum = compute_dim_sum(xs);
    int64_t y_sum = compute_dim_sum(ys);
    
    // The problem asks for the sum over 'iters' iterations. 
    // The baseline resets 'total = 0' inside the loop, meaning it only returns the result of the LAST iteration.
    // However, the baseline code provided says: 
    // for (int iter = 0; iter < iters; ++iter) { total = 0; ... }
    // This means the result is simply the sum for a single iteration.
    
    return x_sum + y_sum;
}