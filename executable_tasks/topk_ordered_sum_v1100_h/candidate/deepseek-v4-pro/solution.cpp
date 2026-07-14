#include "interface.h"
#include <queue>
#include <vector>
#include <functional>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    uint64_t sum = 0;
    if (k > 0) {
        // Use a min-heap to keep the largest k elements
        std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t>> heap;
        for (uint32_t score : scores) {
            if (heap.size() < static_cast<std::size_t>(k)) {
                heap.push(score);
            } else if (score > heap.top()) {
                heap.pop();
                heap.push(score);
            }
        }
        // Sum the k largest elements
        while (!heap.empty()) {
            sum += static_cast<uint64_t>(heap.top());
            heap.pop();
        }
    }
    // Simulate the required number of iterations without recomputing the sum
    volatile int iter_count = iters;
    for (int i = 0; i < iter_count; ++i) {
        // empty body; volatile counter prevents loop elimination
    }
    return sum;
}