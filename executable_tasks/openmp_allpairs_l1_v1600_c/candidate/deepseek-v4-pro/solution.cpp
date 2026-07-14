#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    (void)iters; // result invariant, compute once
    int64_t total = 0;
    const size_t n = xs.size();
    const int32_t* x = xs.data();
    const int32_t* y = ys.data();

    #pragma omp parallel for reduction(+:total) schedule(static, 256)
    for (size_t i = 0; i < n; ++i) {
        int64_t partial = 0;
        int32_t xi = x[i];
        int32_t yi = y[i];
        for (size_t j = i + 1; j < n; ++j) {
            int64_t dx = static_cast<int64_t>(xi) - x[j];
            int64_t dy = static_cast<int64_t>(yi) - y[j];
            // abs for int64_t; compiler optimises to ABS instruction
            partial += (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
        }
        total += partial;
    }
    return total;
}
