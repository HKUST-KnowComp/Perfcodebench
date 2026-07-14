#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    std::size_t n = scores.size();
    if (static_cast<std::size_t>(k) >= n) {
        uint64_t sum = 0;
        for (const auto& x : scores) sum += x;
        return sum;
    }
    std::vector<uint32_t> heap(scores.begin(), scores.begin() + k);
    std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
    for (std::size_t i = k; i < n; ++i) {
        if (scores[i] > heap.front()) {
            std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            heap.back() = scores[i];
            std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
        }
    }
    uint64_t sum = 0;
    for (const auto& x : heap) sum += x;
    return sum;
}