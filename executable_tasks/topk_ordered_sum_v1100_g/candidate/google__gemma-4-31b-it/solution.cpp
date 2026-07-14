#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    if (scores.empty()) return 0;

    size_t n = scores.size();
    size_t k_size = static_cast<size_t>(k);
    if (k_size >= n) {
        uint64_t total_sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            uint64_t current_sum = 0;
            for (uint32_t val : scores) current_sum += val;
            total_sum = current_sum;
        }
        return total_sum;
    }

    std::vector<uint32_t> tmp;
    tmp.reserve(n);
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp.assign(scores.begin(), scores.end());
        
        // Use nth_element to put the k-th largest element at position k-1
        // and all larger elements to the left.
        std::nth_element(tmp.begin(), tmp.begin() + k_size - 1, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (size_t i = 0; i < k_size; ++i) {
            current_sum += tmp[i];
        }
        final_sum = current_sum;
    }

    return final_sum;
}