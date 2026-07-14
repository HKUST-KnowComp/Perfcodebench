#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (iters <= 0 || k <= 0) return 0;
    std::vector<uint32_t> heap;
    heap.reserve(k);
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        heap.clear();
        for (uint32_t val : scores) {
            if (static_cast<int>(heap.size()) < k) {
                heap.push_back(val);
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            } else if (val > heap.front()) {
                std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
                heap.back() = val;
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            }
        }
        sum = 0;
        for (uint32_t v : heap) {
            sum += static_cast<uint64_t>(v);
        }
    }
    return sum;
}
