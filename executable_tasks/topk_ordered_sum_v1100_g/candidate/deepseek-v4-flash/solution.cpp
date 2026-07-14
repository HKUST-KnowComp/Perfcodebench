#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <queue>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    std::size_t n = scores.size();
    std::size_t actual_k = std::min(static_cast<std::size_t>(k), n);
    std::vector<uint32_t> heap;
    heap.reserve(actual_k + 1);
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        heap.clear();
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t val = scores[i];
            if (heap.size() < actual_k) {
                heap.push_back(val);
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            } else if (val > heap[0]) {
                std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
                heap.back() = val;
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            }
        }
        sum = 0;
        for (std::size_t i = 0; i < heap.size(); ++i) {
            sum += static_cast<uint64_t>(heap[i]);
        }
    }
    return sum;
}
