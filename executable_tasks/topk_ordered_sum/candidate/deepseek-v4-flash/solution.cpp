#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0) return 0;

    std::vector<uint32_t> heap;
    heap.reserve(static_cast<std::size_t>(k) + 1);
    uint64_t sum = 0;
    const std::size_t n = scores.size();
    const std::size_t actual_k = static_cast<std::size_t>(k) < n ? static_cast<std::size_t>(k) : n;

    for (int iter = 0; iter < iters; ++iter) {
        heap.clear();
        // Build min-heap of the largest actual_k elements
        if (actual_k == 0) {
            sum = 0;
            continue;
        }
        for (const auto& val : scores) {
            if (heap.size() < actual_k) {
                heap.push_back(val);
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            } else if (val > heap.front()) {
                std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
                heap.pop_back();
                heap.push_back(val);
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            }
        }
        // Sum the heap (top actual_k elements)
        sum = 0;
        for (const auto& v : heap) {
            sum += static_cast<uint64_t>(v);
        }
    }
    return sum;
}
