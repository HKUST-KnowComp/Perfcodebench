#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static inline int64_t sum_abs_pairs_1d(const std::vector<int32_t>& v) {
    const std::size_t n_sz = v.size();
    if (n_sz <= 1) return 0;
    std::vector<int32_t> a(v.begin(), v.end());
    std::sort(a.begin(), a.end());
    const int32_t* p = a.data();
    int64_t n = static_cast<int64_t>(n_sz);
    int64_t s = 0;
    int64_t m = -n + 1; // multiplier starts at -(n-1) and increases by 2 each step
    for (int64_t i = 0; i < n; ++i) {
        s += static_cast<int64_t>(p[i]) * m;
        m += 2;
    }
    return s;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int /*iters*/) {
    const std::size_t n = xs.size();
    if (ys.size() != n) {
        // Fallback: if mismatch, use minimum common length to avoid out-of-bounds
        const std::size_t m = std::min(n, ys.size());
        std::vector<int32_t> xsc(xs.begin(), xs.begin() + m);
        std::vector<int32_t> ysc(ys.begin(), ys.begin() + m);
        int64_t sx = 0, sy = 0;
        if (m <= 1) return 0;
        #pragma omp parallel sections if(m >= 4096)
        {
            #pragma omp section
            { sx = sum_abs_pairs_1d(xsc); }
            #pragma omp section
            { sy = sum_abs_pairs_1d(ysc); }
        }
        return sx + sy;
    }

    if (n <= 1) return 0;

    int64_t sx = 0, sy = 0;
    #pragma omp parallel sections if(n >= 4096)
    {
        #pragma omp section
        { sx = sum_abs_pairs_1d(xs); }
        #pragma omp section
        { sy = sum_abs_pairs_1d(ys); }
    }
    return sx + sy;
}
