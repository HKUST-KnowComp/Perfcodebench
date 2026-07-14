#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;

    const std::size_t n = scores.size();
    const std::size_t kk = static_cast<std::size_t>(k);
    if (kk >= n) {
        uint64_t total = 0;
        for (uint32_t v : scores) total += v;
        return total;
    }

    std::vector<uint32_t> tmp(n);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // copy once per iteration (no reallocations)
        std::copy(scores.begin(), scores.end(), tmp.begin());

        // partial selection: put the k largest at the front
        std::nth_element(tmp.begin(), tmp.begin() + kk, tmp.end(), std::greater<uint32_t>());

        // sort the first k to ensure stable order for exact summation
        std::sort(tmp.begin(), tmp.begin() + kk, std::greater<uint32_t>());

        // accumulate
        sum = 0;
        for (std::size_t i = 0; i < kk; ++i) {
            sum += static_cast<uint64_t>(tmp[i]);
        }
    }
    return sum;
}