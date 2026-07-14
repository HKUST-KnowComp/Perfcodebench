#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (iters <= 0 || k <= 0) return 0;
    
    size_t actual_k = static_cast<size_t>(k);
    if (actual_k > scores.size()) {
        actual_k = scores.size();
    }

    std::vector<uint32_t> tmp = scores;
    if (actual_k < tmp.size()) {
        std::nth_element(tmp.begin(), tmp.begin() + actual_k, tmp.end(), std::greater<uint32_t>());
    }
    
    uint64_t sum = 0;
    for (size_t i = 0; i < actual_k; ++i) {
        sum += tmp[i];
    }
    
    return sum;
}
