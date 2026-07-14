#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    if (static_cast<size_t>(k) >= scores.size()) {
        uint64_t sum = 0;
        for (uint32_t s : scores) sum += s;
        return sum;
    }

    std::vector<uint32_t> tmp;
    tmp.reserve(scores.size());
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;
        // nth_element partitions the array such that the k-th largest element is at index k-1
        // and all elements before it are >= elements after it.
        std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
        
        sum = 0;
        for (int i = 0; i < k; ++i) {
            sum += static_cast<uint64_t>(tmp[static_cast<std::size_t>(i)]);
        }
    }
    return sum;
}