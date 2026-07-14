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
        
        // Use nth_element to partition the top k elements to the front in O(N)
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += tmp[i];
        }
        final_sum = current_sum;
    }
    
    // The original baseline resets sum = 0 inside the loop and returns the sum of the LAST iteration.
    // Based on the baseline code: sum = 0; for(i < k) sum += tmp[i]; return sum;
    // This means iters is likely used for benchmarking or specific simulation, 
    // but only the result of the final iteration is returned.
    return final_sum;
}