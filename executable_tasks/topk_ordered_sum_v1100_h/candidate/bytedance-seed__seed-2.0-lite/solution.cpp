#include "interface.h"
#include <algorithm>
#include <cstring>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (scores.empty() || k <= 0) return 0;
    const std::size_t n = scores.size();
    const std::size_t k_sz = static_cast<std::size_t>(k);
    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;
    const uint32_t* src = scores.data();
    uint32_t* dst = tmp.data();
    for (int iter = 0; iter < iters; ++iter) {
        std::memcpy(dst, src, n * sizeof(uint32_t));
        std::nth_element(tmp.begin(), tmp.begin() + k_sz - 1, tmp.end(), std::greater<uint32_t>());
        sum = 0;
        for (std::size_t i = 0; i < k_sz; ++i) {
            sum += tmp[i];
        }
    }
    return sum;
}