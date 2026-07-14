#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) {
        return 0;
    }
    
    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) {
        target_k = n;
    }

    std::vector<uint32_t> tmp(scores);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Use nth_element to partition the top k elements to the front.
        // This is O(N) on average compared to O(N log N) for full sort.
        std::nth_element(tmp.begin(), tmp.begin() + target_k, tmp.end(), std::greater<uint32_t>());
        
        // Note: nth_element ensures elements from [begin, begin + target_k) 
        // are the largest, but they are not necessarily sorted among themselves.
        // However, for a sum, order does not matter.
        
        // We need to be careful: nth_element with target_k actually places the 
        // (target_k)-th element in its sorted position. To get the top K, 
        // we need the elements from index 0 to target_k - 1.
        // The standard nth_element(first, nth, last) makes 'nth' the element 
        // that would be there if sorted. 
        // To get top K, we use tmp.begin() + target_k - 1.
        
        // Re-running nth_element to ensure the top K are in the first K positions.
        std::nth_element(tmp.begin(), tmp.begin() + target_k - 1, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
        
        // Since the task implies the sum is the same across iterations (scores is const),
        // and we must return the sum after 'iters' iterations, we must actually 
        // perform the work 'iters' times as per the baseline logic.
        // However, the baseline re-copies 'scores' into 'tmp' every time.
        // To maintain correctness with the baseline's behavior of re-copying:
        if (iter < iters - 1) {
            std::copy(scores.begin(), scores.end(), tmp.begin());
        }
    }

    return last_sum;
}