#include "interface.h"

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    const std::size_t n = xs.size();
    if (n == 0) return 0;
    const int32_t* px = xs.data();
    const int32_t* py = ys.data();
    int64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        #pragma omp parallel for schedule(static) reduction(+:total)
        for (std::size_t i = 0; i < n; ++i) {
            const int32_t xi = px[i];
            const int32_t yi = py[i];
            for (std::size_t j = i + 1; j < n; ++j) {
                total += static_cast<int64_t>(std::abs(xi - px[j])) +
                         static_cast<int64_t>(std::abs(yi - py[j]));
            }
        }
    }
    return total;
}