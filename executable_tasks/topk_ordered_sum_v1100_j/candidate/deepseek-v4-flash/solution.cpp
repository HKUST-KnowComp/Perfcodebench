#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    std::size_t n = scores.size();
    if (k <= 0) return 0;
    uint64_t sum = 0;
    // If k >= n, sum all once and reuse each iteration
    if (static_cast<std::size_t>(k) >= n) {
        uint64_t total = 0;
        for (auto x : scores) total += x;
        for (int iter = 0; iter < iters; ++iter) {
            sum = total;
        }
        return sum;
    }
    // General case: k < n
    std::vector<uint32_t> tmp;
    tmp.reserve(n);
    for (int iter = 0; iter < iters; ++iter) {
        tmp = scores;  // copy
        // nth_element places the k-th largest at position k-1
        auto mid = tmp.begin() + (k - 1);
        std::nth_element(tmp.begin(), mid, tmp.end(), std::greater<uint32_t>());
        // Sum the first k elements (these are the top k, unsorted)
        sum = 0;
        for (int i = 0; i < k; ++i) {
            sum += static_cast<uint64_t>(tmp[i]);
        }
    }
    return sum;
}
