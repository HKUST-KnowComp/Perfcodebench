#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    const std::size_t N = xs.size();
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        #pragma omp parallel for reduction(+:total) schedule(guided)
        for (std::size_t i = 0; i < N; ++i) {
            const int32_t xi = xs[i];
            const int32_t yi = ys[i];
            for (std::size_t j = i + 1; j < N; ++j) {
                total += std::abs(static_cast<int64_t>(xi - xs[j])) +
                         std::abs(static_cast<int64_t>(yi - ys[j]));
            }
        }
    }
    return total;
}
