#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) {
        return 0;
    }
    
    // Ensure k does not exceed vector size
    int actual_k = (k > static_cast<int>(scores.size())) ? static_cast<int>(scores.size()) : k;
    
    // Pre-allocate buffer to avoid repeated allocations in the loop
    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());
    
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;
        
        // Use nth_element to partially sort the array such that the top k elements 
        // are moved to the front. Average complexity: O(N).
        std::nth_element(tmp.begin(), tmp.begin() + actual_k, tmp.end(), std::greater<uint32_t>());
        
        // Note: nth_element puts the element that would be at the nth position in a sorted range 
        // in that position, and all elements before it are >= (if using greater) or <= (if using less).
        // However, the standard nth_element(first, nth, last) ensures elements in [first, nth) 
        // satisfy the predicate relative to the element at nth. 
        // To get the top k, we use nth_element with the k-th position.
        
        // Re-adjusting logic: nth_element(begin, begin + k, end) makes [begin, begin + k) 
        // contain the largest k elements if using std::greater.
        // But the iterator passed to nth_element is the element that will be at that position.
        // To get exactly k elements, we use tmp.begin() + actual_k - 1 if we want the k-th element.
        // Actually, std::nth_element(begin, begin + k, end) results in [begin, begin + k) 
        // being the elements that would be there in a sorted sequence.
        
        // Correct usage for top k: 
        // std::nth_element(tmp.begin(), tmp.begin() + actual_k, tmp.end(), std::greater<uint32_t>());
        // This puts the (actual_k)-th largest element at tmp.begin() + actual_k.
        // We want the top k, so we need the partition at index actual_k - 1.
        
        std::nth_element(tmp.begin(), tmp.begin() + (actual_k - 1), tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (int i = 0; i < actual_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
    }

    return last_sum;
}