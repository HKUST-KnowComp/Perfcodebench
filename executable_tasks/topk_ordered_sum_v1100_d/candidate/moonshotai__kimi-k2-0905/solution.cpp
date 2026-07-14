#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    std::vector<uint32_t> tmp(scores.size());
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::copy(scores.begin(), scores.end(), tmp.begin());
        std::nth_element(tmp.begin(), tmp.begin() + k - 1, tmp.end(), std::greater<uint32_t>());
        uint32_t kth = tmp[k - 1];
        sum = 0;
        for (size_t i = 0; i < tmp.size(); ++i) {
            if (tmp[i] >= kth) sum += static_cast<uint64_t>(tmp[i]);
        }
    }
    return sum;
}