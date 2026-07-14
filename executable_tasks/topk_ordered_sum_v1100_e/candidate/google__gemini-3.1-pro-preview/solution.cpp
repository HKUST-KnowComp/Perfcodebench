#include "interface.h"
#include <algorithm>
#include <numeric>
#include <vector>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    size_t n = scores.size();
    if (static_cast<size_t>(k) >= n) {
        uint64_t total_sum = 0;
        for (uint32_t val : scores) {
            total_sum += val;
        }
        return total_sum;
    }

    // Reuse buffer across iterations to minimize allocation overhead
    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input scores into the working buffer
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // Use nth_element to partition the top k elements to the front.
        // This is O(N) on average compared to O(N log N) for std::sort.
        // We use std::greater to ensure the largest elements are at the beginning.
        auto target_it = tmp.begin() + k;
        std::nth_element(tmp.begin(), target_it - 1, tmp.end(), std::greater<uint32_t>());

        // Sum the first k elements
        sum = 0;
        for (int i = 0; i < k; ++i) {
            sum += static_cast<uint64_t>(tmp[i]);
        }
    }

    return sum;
}