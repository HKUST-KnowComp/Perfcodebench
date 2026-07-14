#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    if (iters <= 0) return 0;
    const std::size_t n = xs.size();
    if (n < 2) return 0;
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total) schedule(guided)
    for (std::size_t i = 0; i < n; ++i) {
        const int32_t xi = xs[i];
        const int32_t yi = ys[i];
        for (std::size_t j = i + 1; j < n; ++j) {
            total += static_cast<int64_t>(std::abs(xi - xs[j])) +
                     static_cast<int64_t>(std::abs(yi - ys[j]));
        }
    }
    return total;
}