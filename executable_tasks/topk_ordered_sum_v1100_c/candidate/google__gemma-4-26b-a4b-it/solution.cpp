#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0) return 0;
    
    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) target_k = n;

    // Pre-allocate buffer to avoid repeated allocations in the loop
    std::vector<uint32_t> tmp(scores);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Copy scores into the reusable buffer
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // Use nth_element to partially sort the array such that the top k elements 
        // are moved to the front. Average complexity: O(N).
        // We use std::greater to get the largest elements at the beginning.
        std::nth_element(tmp.begin(), tmp.begin() + target_k, tmp.end(), std::greater<uint32_t>());

        // Sum the first k elements
        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
    }

    return last_sum;
}