#include "interface.h"
#include <algorithm>
#include <functional>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0) return 0;
    std::size_t kk = static_cast<std::size_t>(k);
    if (kk > scores.size()) kk = scores.size();
    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;
        std::nth_element(tmp.begin(), tmp.begin() + (kk - 1), tmp.end(), std::greater<uint32_t>{});
        uint64_t s = 0;
        for (std::size_t i = 0; i < kk; ++i) {
            s += tmp[i];
        }
        sum = s;
    }
    return sum;
}