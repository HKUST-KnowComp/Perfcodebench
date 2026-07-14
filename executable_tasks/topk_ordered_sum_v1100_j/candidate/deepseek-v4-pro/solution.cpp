#include "interface.h"
#include <queue>
#include <cstdint>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    (void)iters; // result is independent of iterations
    if (k <= 0) return 0;
    std::size_t n = scores.size();
    if (static_cast<std::size_t>(k) >= n) {
        uint64_t total = 0;
        for (std::size_t i = 0; i < n; ++i) total += scores[i];
        return total;
    }
    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> min_heap;
    for (std::size_t i = 0; i < n; ++i) {
        uint32_t val = scores[i];
        if (min_heap.size() < static_cast<std::size_t>(k)) {
            min_heap.push(val);
        } else if (val > min_heap.top()) {
            min_heap.pop();
            min_heap.push(val);
        }
    }
    uint64_t sum = 0;
    while (!min_heap.empty()) {
        sum += static_cast<uint64_t>(min_heap.top());
        min_heap.pop();
    }
    return sum;
}