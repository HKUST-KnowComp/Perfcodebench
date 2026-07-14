#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
    uint64_t total_sum = 0;
    
    // Pre-calculate the intersection sum once per iteration to satisfy the 'iters' requirement.
    // Since the arrays are sorted and unique, a two-pointer approach is O(N + M).
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        size_t i = 0;
        size_t j = 0;
        const size_t n = lhs.size();
        const size_t m = rhs.size();

        while (i < n && j < m) {
            if (lhs[i] == rhs[j]) {
                current_iter_sum += static_cast<uint64_t>(lhs[i]);
                ++i;
                ++j;
            } else if (lhs[i] < rhs[j]) {
                ++i;
            } else {
                ++j;
            }
        }
        total_sum = current_iter_sum;
    }
    
    return total_sum;
}