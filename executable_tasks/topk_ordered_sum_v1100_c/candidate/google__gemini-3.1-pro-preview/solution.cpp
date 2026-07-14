#include "interface.h"
#include <algorithm>
#include <vector>
#include <queue>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty()) return 0;
    
    size_t n = scores.size();
    if (k > (int)n) k = (int)n;

    uint64_t final_sum = 0;

    // The loop is for benchmarking; since scores is const, the result is identical each iteration.
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;

        if (k == (int)n) {
            for (uint32_t val : scores) {
                current_sum += val;
            }
        } else if (k <= 256) {
            // Optimization for small k: use a stack-allocated min-heap
            uint32_t heap[256];
            for (int i = 0; i < k; ++i) {
                heap[i] = scores[i];
            }
            std::make_heap(heap, heap + k, std::greater<uint32_t>());

            for (size_t i = k; i < n; ++i) {
                if (scores[i] > heap[0]) {
                    std::pop_heap(heap, heap + k, std::greater<uint32_t>());
                    heap[k - 1] = scores[i];
                    std::push_heap(heap, heap + k, std::greater<uint32_t>());
                }
            }

            for (int i = 0; i < k; ++i) {
                current_sum += heap[i];
            }
        } else {
            // Fallback for larger k: use nth_element which is O(N)
            std::vector<uint32_t> tmp = scores;
            std::nth_element(tmp.begin(), tmp.begin() + k, tmp.end(), std::greater<uint32_t>());
            for (int i = 0; i < k; ++i) {
                current_sum += tmp[i];
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}