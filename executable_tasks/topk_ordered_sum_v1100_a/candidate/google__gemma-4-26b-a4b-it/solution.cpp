#include "interface.h"
#include <algorithm>
#include <vector>
#include <numeric>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    if (k <= 0 || scores.empty() || iters <= 0) {
        return 0;
    }

    size_t n = scores.size();
    size_t target_k = static_cast<size_t>(k);
    if (target_k > n) {
        target_k = n;
    }

    // Pre-allocate buffer to avoid repeated allocations inside the loop
    std::vector<uint32_t> tmp(scores);
    uint64_t last_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // We must reset/copy the scores if the algorithm modifies tmp.
        // Since nth_element partially sorts, we need a fresh copy each time.
        // However, if the input scores are constant, we can optimize the copy.
        std::vector<uint32_t> current_iter_vec = scores;

        // std::nth_element puts the (target_k-1)-th largest element in its correct position
        // and ensures all elements before it are >= it (if using std::greater).
        std::nth_element(current_iter_vec.begin(), 
                         current_iter_vec.begin() + target_k - 1, 
                         current_iter_vec.end(), 
                         std::greater<uint32_t>());

        uint64_t current_sum = 0;
        for (size_t i = 0; i < target_k; ++i) {
            current_sum += static_cast<uint64_t>(current_iter_vec[i]);
        }
        last_sum = current_sum;
    }

    return last_sum;
}