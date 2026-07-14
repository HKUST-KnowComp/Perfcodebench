#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    std::size_t n = xs.size();
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total) schedule(dynamic)
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            int64_t dx = (int64_t)xs[i] - (int64_t)xs[j];
            int64_t dy = (int64_t)ys[i] - (int64_t)ys[j];
            total += (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
        }
    }
    return total;
}