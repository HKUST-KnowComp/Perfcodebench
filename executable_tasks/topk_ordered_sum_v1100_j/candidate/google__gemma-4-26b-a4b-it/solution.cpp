#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0) return 0;
    
    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) target_k = n;

    std::vector<uint32_t> tmp(scores);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Use nth_element to find the top k elements in O(N) average time
        std::nth_element(tmp.begin(), tmp.begin() + target_k, tmp.end(), std::greater<uint32_t>());
        
        // Note: nth_element puts the k-th element at the position, and elements 
        // greater than or equal to it before it (due to std::greater).
        // However, the standard nth_element behavior for [first, nth, last) 
        // ensures that the element at 'nth' is the one that would be there in a sorted sequence.
        // To get the top k, we need the range [begin, begin + target_k).
        
        // Re-running nth_element to ensure the partition is correct for the top k
        std::nth_element(tmp.begin(), tmp.begin() + target_k - 1, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
        
        // Since the task implies the scores are static and we just repeat the process,
        // and the result must match the reference, we must reset tmp if it were modified 
        // in a way that affects subsequent iterations. 
        // But nth_element only reorders. To ensure correctness across iterations 
        // if the input were to change (though here it's constant), we reset.
        tmp = scores;
    }

    return last_sum;
}