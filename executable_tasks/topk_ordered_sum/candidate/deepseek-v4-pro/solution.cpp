#include "interface.h"
#include <algorithm>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (iters <= 0 || k <= 0) {
        return 0;
    }
    std::size_t n = scores.size();
    if (k >= static_cast<int>(n)) {
        uint64_t total = 0;
        for (std::size_t i = 0; i < n; ++i) {
            total += scores[i];
        }
        return total;
    }
    std::vector<uint32_t> tmp(scores);
    std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
    return std::accumulate(tmp.begin(), tmp.begin() + k, static_cast<uint64_t>(0));
}
