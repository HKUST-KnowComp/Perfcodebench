#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;
    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) target_k = n;

    // Pre-allocate buffer to avoid repeated allocations in the loop
    std::vector<uint32_t> tmp(scores);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset tmp to original scores for each iteration
        // Using std::copy is generally faster than assignment for pre-allocated vectors
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // std::nth_element partially sorts the range such that the element at 
        // the nth position is the one that would be there in a sorted sequence.
        // We use std::greater to bring the largest elements to the front.
        std::nth_element(tmp.begin(), tmp.begin() + target_k, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
    }

    return last_sum;
}