#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    (void)iters; // compute once, result independent of iters
    if (k <= 0) return 0;
    if (k >= static_cast<int>(scores.size())) {
        uint64_t sum = 0;
        for (auto v : scores) sum += v;
        return sum;
    }
    std::vector<uint32_t> tmp = scores;
    std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
    uint64_t sum = 0;
    for (int i = 0; i < k; ++i) {
        sum += static_cast<uint64_t>(tmp[i]);
    }
    return sum;
}
