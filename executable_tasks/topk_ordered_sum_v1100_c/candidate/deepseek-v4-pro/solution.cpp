#include "interface.h"

#include <queue>
#include <functional>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;
    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> min_heap;
    for (uint32_t s : scores) {
        if (static_cast<int>(min_heap.size()) < k) {
            min_heap.push(s);
        } else if (s > min_heap.top()) {
            min_heap.pop();
            min_heap.push(s);
        }
    }
    uint64_t sum = 0;
    while (!min_heap.empty()) {
        sum += static_cast<uint64_t>(min_heap.top());
        min_heap.pop();
    }
    return sum;
}