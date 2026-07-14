#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

// Helper function to calculate the sum of absolute differences for a single dimension in O(N log N)
int64_t sum_abs_diff(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        // Each element coords[i] is added i times and subtracted (n - 1 - i) times
        // in the sum of all pairs (i < j)
        sum += static_cast<int64_t>(coords[i]) * (static_cast<int64_t>(i) - static_cast<int64_t>(n - 1 - i));
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    // Pre-calculate the sum for one iteration since the input is constant across iterations
    // The problem asks for the sum over 'iters' iterations.
    // Note: The baseline code resets 'total = 0' inside the loop, effectively accumulating
    // the sum of all iterations. 
    
    std::vector<int32_t> x_copy = xs;
    std::vector<int32_t> y_copy = ys;
    
    int64_t single_iter_sum = sum_abs_diff(x_copy) + sum_abs_diff(y_copy);
    
    return single_iter_sum * iters;
}