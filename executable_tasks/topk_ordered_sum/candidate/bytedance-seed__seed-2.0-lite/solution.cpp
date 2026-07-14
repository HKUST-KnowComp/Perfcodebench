#include "interface.h"
#include <algorithm>
#include <numeric>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0 || iters <= 0) {
        return 0;
    }
    const std::size_t n = scores.size();
    const std::size_t take = std::min(static_cast<std::size_t>(k), n);
    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::copy(scores.begin(), scores.end(), tmp.begin());
        if (take < n) {
            std::nth_element(tmp.begin(), tmp.begin() + take - 1, tmp.end(), std::greater<uint32_t>());
        }
        sum = std::accumulate(tmp.begin(), tmp.begin() + take, static_cast<uint64_t>(0));
    }
    return sum;
}