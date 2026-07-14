#include "interface.h"
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    std::size_t n = xs.size();
    #pragma omp parallel for reduction(+:total)
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            total += static_cast<int64_t>(std::abs(xs[i] - xs[j])) +
                     static_cast<int64_t>(std::abs(ys[i] - ys[j]));
        }
    }
    return total;
}
