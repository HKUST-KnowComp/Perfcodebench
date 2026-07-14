#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;
    if (static_cast<std::size_t>(k) > scores.size()) k = static_cast<int>(scores.size());

    std::vector<uint32_t> tmp = scores;
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Use nth_element to partially sort the array such that the top k elements 
        // are moved to the front (in no particular order).
        // We use std::greater to get the largest elements.
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        
        // Note: nth_element puts the (n)-th element in its sorted position and 
        // ensures all elements before it satisfy the predicate relative to it.
        // To get the top k, we actually need the element at index k-1 to be the pivot.
        std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
        
        // Since the task implies the scores array is constant across iters, 
        // and we are modifying 'tmp', we must reset 'tmp' if the logic requires 
        // fresh data. However, the baseline re-copies 'scores' every iteration.
        // To maintain correctness with the baseline's behavior:
        tmp = scores;
    }

    return last_sum;
}