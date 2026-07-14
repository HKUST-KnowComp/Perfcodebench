#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) {
        return 0;
    }
    
    // Ensure k does not exceed vector size
    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) {
        target_k = n;
    }

    std::vector<uint32_t> tmp(n);
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Copy scores to tmp
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // Use nth_element to partially sort the array such that the top k elements 
        // are in the first k positions. Average complexity: O(N).
        // We use std::greater to put largest elements at the beginning.
        std::nth_element(tmp.begin(), tmp.begin() + target_k, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        final_sum = current_sum;
    }

    return final_sum;
}