#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    int64_t single_sum = 0;
    std::size_t n = xs.size();
    const int32_t* x_ptr = xs.data();
    const int32_t* y_ptr = ys.data();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            single_sum += static_cast<int64_t>(std::abs(x_ptr[i] - x_ptr[j])) +
                          static_cast<int64_t>(std::abs(y_ptr[i] - y_ptr[j]));
        }
    }
    return single_sum * iters;
}
