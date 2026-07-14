#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    if (k >= (int)scores.size()) {
        uint64_t total_sum = 0;
        for (uint32_t s : scores) total_sum += s;
        return total_sum * iters;
    }

    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp.assign(scores.begin(), scores.end());
        
        // Use nth_element to put the k-th largest element at position k-1
        // and all larger elements before it.
        std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += tmp[i];
        }
        final_sum = current_sum; // The prompt says "Return the exact uint64 sum", and the baseline resets sum = 0 inside the loop. 
        // Wait, the baseline does: sum = 0; for(i < k) sum += tmp[i]; then returns sum. 
        // This means it only returns the sum of the LAST iteration. 
        // Let's re-read the baseline: "sum = 0; for (int i = 0; i < k; ++i) { sum += ... }" is inside the loop.
        // The baseline returns the sum of the k largest elements of the last iteration.
    }
    
    // To match baseline behavior exactly: the baseline resets sum to 0 every iteration.
    // Therefore, it returns the sum of the k largest elements of the final iteration.
    // However, the baseline's loop structure is: 
    // for (iter < iters) { tmp = scores; sort; sum = 0; for (i < k) sum += tmp[i]; }
    // return sum;
    // This means the result is just the top-k sum of the scores array, calculated 'iters' times.
    
    // Let's optimize for that specific behavior.
    std::vector<uint32_t> final_tmp = scores;
    std::nth_element(final_tmp.begin(), final_tmp.begin() + k - 1, final_tmp.end(), std::greater<uint32_t>());
    uint64_t result = 0;
    for (int i = 0; i < k; ++i) result += final_tmp[i];
    
    // If iters is 0, the baseline returns 0. If iters > 0, it returns the sum of the last run.
    return (iters > 0) ? result : 0;
}