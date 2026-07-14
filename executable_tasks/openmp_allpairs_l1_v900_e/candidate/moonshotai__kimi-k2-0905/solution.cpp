#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs,
                        const std::vector<int32_t>& ys,
                        int iters) {
    const std::size_t n = xs.size();
    if (n < 2) return 0;

    // Pre-compute the difference vectors (read-only, shared)
    std::vector<int64_t> dx(n * n);
    std::vector<int64_t> dy(n * n);
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            dx[i * n + j] = std::abs(xs[i] - xs[j]);
            dy[i * n + j] = std::abs(ys[i] - ys[j]);
        }
    }

    // Accumulate across iterations in parallel
    int64_t total = 0;
    #pragma omp parallel reduction(+:total)
    {
        const int tid       = omp_get_thread_num();
        const int nthreads  = omp_get_num_threads();
        const int64_t chunk = (iters + nthreads - 1) / nthreads;
        const int64_t start = tid * chunk;
        const int64_t end   = (start + chunk < iters) ? start + chunk : iters;

        for (int64_t iter = start; iter < end; ++iter) {
            int64_t local = 0;
            std::size_t j = 1;
            // unroll 4×
            for (std::size_t i = 0; i + 3 < n; i += 4) {
                #define DO(k) do { \
                    const std::size_t base = i + k; \
                    for (; j < n; ++j) { \
                        local += dx[base * n + j] + dy[base * n + j]; \
                    } \
                    j = base + 1; \
                } while (0)
                DO(0); DO(1); DO(2); DO(3);
                #undef DO
            }
            // tail
            for (std::size_t i = (n & ~3U); i < n; ++i) {
                for (std::size_t j = i + 1; j < n; ++j) {
                    local += dx[i * n + j] + dy[i * n + j];
                }
            }
            total += local;
        }
    }
    return total;
}