#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    size_t n = scores.size();
    if (k > (int)n) k = (int)n;

    std::vector<uint32_t> tmp;
    tmp.reserve(n);
    uint64_t total_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp.assign(scores.begin(), scores.end());
        
        // Use nth_element to put the k-th largest element in its place
        // and all larger elements to its left.
        std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
        
        uint64_t current_sum = 0;
        for (int i = 0; i < k; ++i) {
            current_sum += tmp[i];
        }
        total_sum = current_sum;
    }
    
    return total_sum;
}