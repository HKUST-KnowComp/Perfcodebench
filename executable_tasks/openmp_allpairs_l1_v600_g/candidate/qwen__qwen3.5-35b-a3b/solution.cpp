#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    std::size_t n = xs.size();
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        #pragma omp parallel for reduction(+:total)
        for (std::size_t i = 0; i < n; ++i) {
            int64_t xi = xs[i];
            int64_t yi = ys[i];
            for (std::size_t j = i + 1; j < n; ++j) {
                total += std::abs(xi - static_cast<int64_t>(xs[j])) +
                         std::abs(yi - static_cast<int64_t>(ys[j]));
            }
        }
    }
    return total;
}