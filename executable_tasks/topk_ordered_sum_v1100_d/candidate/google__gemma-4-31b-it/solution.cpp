#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    if (k >= static_cast<int>(scores.size())) {
        uint64_t total_sum = 0;
        for (uint32_t val : scores) total_sum += val;
        return total_sum * iters;
    }

    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp.assign(scores.begin(), scores.end());
        
        // Use nth_element to partition the top k elements to the front
        // Complexity: O(N) average
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += tmp[i];
        }
        final_sum = current_sum; // The prompt implies returning the sum of the last iteration or a specific behavior; baseline resets sum each iter.
    }
    
    // Based on baseline: sum = 0 is inside the loop, so it returns the sum of the LAST iteration.
    // However, the baseline logic is slightly ambiguous. If it's meant to be the sum of the last run:
    return final_sum;
}