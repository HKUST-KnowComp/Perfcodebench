#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;
    int n = static_cast<int>(scores.size());
    if (k > n) k = n;

    std::vector<uint32_t> tmp(scores.size());
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Copy scores to tmp to avoid modifying the original input
        std::copy(scores.begin(), scores.end(), tmp.begin());
        
        // Use nth_element to partition the top k elements to the front.
        // This is O(N) on average, compared to O(N log N) for full sort.
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        final_sum = current_sum;
    }

    return final_sum;
}