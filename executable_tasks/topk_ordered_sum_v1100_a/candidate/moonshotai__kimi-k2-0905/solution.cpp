#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    std::vector<uint32_t> tmp;
    uint64_t sum = 0;
    const std::size_t kk = static_cast<std::size_t>(k);
    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;
        // partial sort to bring k largest to front
        std::nth_element(tmp.begin(), tmp.begin() + kk, tmp.end(), std::greater<uint32_t>());
        sum = 0;
        for (std::size_t i = 0; i < kk; ++i) {
            sum += static_cast<uint64_t>(tmp[i]);
        }
    }
    return sum;
}