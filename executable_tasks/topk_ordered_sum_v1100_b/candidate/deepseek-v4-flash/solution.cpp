#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t topk_sum(const std::vector<uint32_t>& scores, int k, int iters) {
    const size_t n = scores.size();
    size_t kk = static_cast<size_t>(k);
    if (kk > n) kk = n;
    if (kk == 0) {
        return 0;  // sum is zero for every iteration
    }

    // Preallocate temporary storage (capacity will not change across iterations)
    std::vector<uint32_t> tmp;
    tmp.reserve(n);

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        tmp.resize(n);
        std::copy(scores.begin(), scores.end(), tmp.begin());

        if (kk == n) {
            // All elements must be summed
            sum = 0;
            for (size_t i = 0; i < n; ++i) {
                sum += tmp[i];
            }
        } else {
            // Quickselect: place the k-th largest element at index kk-1.
            // After this, the first kk elements are the largest k elements (unsorted).
            std::nth_element(tmp.begin(), tmp.begin() + kk - 1, tmp.end(),
                             std::greater<uint32_t>());
            sum = 0;
            for (size_t i = 0; i < kk; ++i) {
                sum += tmp[i];
            }
        }
    }

    return sum;
}
