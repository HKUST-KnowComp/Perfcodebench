#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstring>

/**
 * Optimized top-k sum implementation.
 * 
 * Strategy:
 * 1. Use std::nth_element instead of std::sort to achieve O(N) average time complexity 
 *    per iteration instead of O(N log N).
 * 2. Pre-allocate a temporary buffer once and reuse it across all iterations to 
 *    minimize memory allocation overhead.
 * 3. Use std::copy (which often maps to memcpy) for fast data transfer from the 
 *    input scores to the working buffer.
 * 4. Calculate the sum of the first k elements after partitioning.
 */
uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0) {
        return 0;
    }

    size_t n = scores.size();
    size_t top_k = static_cast<size_t>(k);
    if (top_k > n) {
        top_k = n;
    }

    // Pre-allocate buffer to avoid repeated allocations in the loop
    std::vector<uint32_t> tmp(n);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input data to working buffer
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // Partition the array so that the top k elements are in the first k positions.
        // std::nth_element is O(N) on average.
        std::nth_element(tmp.begin(), tmp.begin() + top_k, tmp.end(), std::greater<uint32_t>());

        // Sum the top k elements
        uint64_t current_sum = 0;
        for (size_t i = 0; i < top_k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
    }

    return last_sum;
}
