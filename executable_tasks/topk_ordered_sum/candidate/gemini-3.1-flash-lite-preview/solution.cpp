#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    if (static_cast<size_t>(k) >= scores.size()) {
        uint64_t total = 0;
        for (uint32_t s : scores) total += s;
        return total;
    }

    std::vector<uint32_t> tmp = scores;
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        if (iter > 0) {
            tmp = scores;
        }
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        sum = 0;
        for (int i = 0; i < k; ++i) {
            sum += static_cast<uint64_t>(tmp[static_cast<size_t>(i)]);
        }
    }
    return sum;
}