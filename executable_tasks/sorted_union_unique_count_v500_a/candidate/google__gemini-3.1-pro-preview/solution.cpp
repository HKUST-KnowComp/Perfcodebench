#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

/**
 * Optimized sorted union unique count.
 * Since the input vectors 'left' and 'right' are already sorted, we can find the 
 * count of distinct elements in the union in O(N + M) time using a two-pointer approach.
 * This avoids the O((N+M) log (N+M)) cost of std::sort and the overhead of allocating 
 * a temporary merged vector.
 */
uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
    
    uint64_t final_count = 0;
    const size_t n_left = left.size();
    const size_t n_right = right.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        size_t i = 0;
        size_t j = 0;
        
        uint32_t last_val = 0;
        bool has_last = false;

        while (i < n_left && j < n_right) {
            uint32_t current;
            uint32_t l_val = left[i];
            uint32_t r_val = right[j];

            if (l_val < r_val) {
                current = l_val;
                i++;
            } else if (r_val < l_val) {
                current = r_val;
                j++;
            } else {
                current = l_val;
                i++;
                j++;
            }

            if (!has_last || current != last_val) {
                count++;
                last_val = current;
                has_last = true;
            }
        }

        while (i < n_left) {
            uint32_t current = left[i++];
            if (!has_last || current != last_val) {
                count++;
                last_val = current;
                has_last = true;
            }
        }

        while (j < n_right) {
            uint32_t current = right[j++];
            if (!has_last || current != last_val) {
                count++;
                last_val = current;
                has_last = true;
            }
        }
        
        final_count = count;
    }

    return final_count;
}
