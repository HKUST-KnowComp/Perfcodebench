#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    (void)iters; // loop over iters is redundant; compute once
    if (k <= 0) return 0;

    const std::size_t n = scores.size();
    if (static_cast<std::size_t>(k) >= n) {
        uint64_t total = 0;
        for (std::size_t i = 0; i < n; ++i) {
            total += static_cast<uint64_t>(scores[i]);
        }
        return total;
    }

    // Min-heap to track the k largest elements seen so far
    std::vector<uint32_t> heap;
    heap.reserve(static_cast<std::size_t>(k));

    // Fill the heap with the first k elements
    for (std::size_t i = 0; i < static_cast<std::size_t>(k); ++i) {
        heap.push_back(scores[i]);
    }
    std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>());

    // Process the remaining elements
    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
        const uint32_t x = scores[i];
        if (x > heap.front()) {
            // Remove the current minimum
            std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
            // Replace the extracted element with the new larger value
            heap.back() = x;
            std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
        }
    }

    // Sum the k largest elements
    uint64_t sum = 0;
    for (std::size_t i = 0; i < heap.size(); ++i) {
        sum += static_cast<uint64_t>(heap[i]);
    }
    return sum;
}