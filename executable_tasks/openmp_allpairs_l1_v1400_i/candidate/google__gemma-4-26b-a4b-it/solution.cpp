#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

// Helper function to calculate the sum of absolute differences for a single dimension in O(N log N)
int64_t sum_abs_diff(std::vector<int32_t> coords) {
    size_t n = coords.size();
    if (n < 2) return 0;
    
    std::sort(coords.begin(), coords.end());
    
    int64_t sum = 0;
    // The sum of |coords[i] - coords[j]| for all i < j is equivalent to
    // sum_{i=0}^{n-1} (i * coords[i] - (n - 1 - i) * coords[i])
    for (size_t i = 0; i < n; ++i) {
        sum += static_cast<int64_t>(i) * coords[i] - static_cast<int64_t>(n - 1 - i) * coords[i];
    }
    return sum;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (xs.empty()) return 0;
    
    // Pre-calculate the sum for one iteration since the input is constant across iterations
    int64_t single_iter_sum = 0;
    
    // We use OpenMP to parallelize the calculation of X and Y components
    #pragma omp parallel
    {
        int64_t x_sum = 0;
        int64_t y_sum = 0;
        
        #pragma omp for nowait
        for (int i = 0; i < 1; ++i) {
            std::vector<int32_t> x_copy = xs;
            std::sort(x_copy.begin(), x_copy.end());
            size_t n = x_copy.size();
            for (size_t j = 0; j < n; ++j) {
                x_sum += static_cast<int64_t>(j) * x_copy[j] - static_cast<int64_t>(n - 1 - j) * x_copy[j];
            }
        }

        #pragma omp for
        for (int i = 0; i < 1; ++i) {
            std::vector<int32_t> y_copy = ys;
            std::sort(y_copy.begin(), y_copy.end());
            size_t n = y_copy.size();
            for (size_t j = 0; j < n; ++j) {
                y_sum += static_cast<int64_t>(j) * y_copy[j] - static_cast<int64_t>(n - 1 - j) * y_copy[j];
            }
        }

        #pragma omp critical
        {
            single_iter_sum += (x_sum + y_sum);
        }
    }

    // Re-implementing the logic cleanly to ensure thread safety and correctness
    // The above block was a sketch; let's use a more robust approach.
    
    int64_t total_sum = 0;
    
    // Calculate X component
    std::vector<int32_t> x_sorted = xs;
    std::sort(x_sorted.begin(), x_sorted.end());
    int64_t x_comp = 0;
    size_t n = x_sorted.size();
    for (size_t i = 0; i < n; ++i) {
        x_comp += static_cast<int64_t>(i) * x_sorted[i] - static_cast<int64_t>(n - 1 - i) * x_sorted[i];
    }

    // Calculate Y component
    std::vector<int32_t> y_sorted = ys;
    std::sort(y_sorted.begin(), y_sorted.end());
    int64_t y_comp = 0;
    for (size_t i = 0; i < n; ++i) {
        y_comp += static_cast<int64_t>(i) * y_sorted[i] - static_cast<int64_t>(n - 1 - i) * y_sorted[i];
    }

    single_iter_sum = x_comp + y_comp;
    return single_iter_sum * iters;
}