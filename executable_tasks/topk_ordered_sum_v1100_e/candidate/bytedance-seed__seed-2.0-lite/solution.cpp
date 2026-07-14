#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    const std::size_t n = scores.size();
    if (n == 0 || k <= 0) return 0;
    const std::size_t take = std::min(static_cast<std::size_t>(k), n);
    
    // Preallocate temporary buffer once to avoid per-iteration heap allocations
    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset buffer to original input values
        std::copy(scores.begin(), scores.end(), tmp.begin());
        // Partition top `take` largest elements to start of tmp, O(n) average time
        std::nth_element(tmp.begin(), tmp.begin() + take, tmp.end(), std::greater<uint32_t>());
        // Calculate sum of top k elements
        sum = 0;
        for (std::size_t i = 0; i < take; ++i) {
            sum += static_cast<uint64_t>(tmp[i]);
        }
    }
    return sum;
}