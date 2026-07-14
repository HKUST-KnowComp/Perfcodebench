#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;
    if (k > static_cast<int>(scores.size())) k = static_cast<int>(scores.size());

    uint64_t last_sum = 0;
    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());

    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;
        // Use nth_element to partially sort the array such that the top k elements 
        // are moved to the front in O(N) average time.
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += static_cast<uint64_t>(tmp[i]);
        }
        last_sum = current_sum;
    }
    return last_sum;
}