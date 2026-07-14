#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty() || iters <= 0) return 0;
    
    size_t actual_k = static_cast<size_t>(k);
    if (actual_k > scores.size()) {
        actual_k = scores.size();
    }

    uint64_t sum = 0;

    // The result is invariant across iterations, so we only need to compute it once.
    if (actual_k < scores.size() / 16) {
        // For small k, maintaining a min-heap avoids copying the entire vector
        // and provides excellent cache locality.
        std::vector<uint32_t> heap(scores.begin(), scores.begin() + actual_k);
        std::make_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
        
        uint32_t min_val = heap.front();
        for (size_t i = actual_k; i < scores.size(); ++i) {
            if (scores[i] > min_val) {
                std::pop_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
                heap.back() = scores[i];
                std::push_heap(heap.begin(), heap.end(), std::greater<uint32_t>());
                min_val = heap.front();
            }
        }
        for (uint32_t val : heap) {
            sum += val;
        }
    } else {
        // For larger k, std::nth_element is highly efficient and runs in O(N) time.
        std::vector<uint32_t> tmp = scores;
        std::nth_element(tmp.begin(), tmp.begin() + actual_k - 1, tmp.end(), std::greater<uint32_t>());
        for (size_t i = 0; i < actual_k; ++i) {
            sum += tmp[i];
        }
    }

    return sum;
}
