#include "interface.h"
#include <algorithm>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    std::vector<uint32_t> buf(scores.size());
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::copy(scores.begin(), scores.end(), buf.begin());
        auto mid = buf.begin() + k;
        std::nth_element(buf.begin(), mid, buf.end(), std::greater<uint32_t>());
        sum = std::accumulate(buf.begin(), mid, uint64_t(0));
    }
    return sum;
}