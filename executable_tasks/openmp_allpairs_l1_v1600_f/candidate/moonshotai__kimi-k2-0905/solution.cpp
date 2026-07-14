#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstdlib>
#include <omp.h>

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs,
                        const std::vector<int32_t>& ys,
                        int iters)
{
    const std::size_t n = xs.size();
    if (n < 2) return 0;

    int64_t final_total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        int64_t iter_total = 0;

        #pragma omp parallel reduction(+:iter_total) schedule(guided)
        {
            const int tid        = omp_get_thread_num();
            const int num_threads = omp_get_num_threads();

            // Each thread owns a local 64-bit accumulator
            int64_t local = 0;

            // Triangular loop split across threads
            #pragma omp for nowait
            for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(n); ++i) {
                const int32_t xi = xs[i];
                const int32_t yi = ys[i];

                int32_t inner_sum = 0;     // 32-bit safe for n ≤ ~2M
                for (std::size_t j = i + 1; j < n; ++j) {
                    inner_sum += std::abs(xi - xs[j]) + std::abs(yi - ys[j]);
                }
                local += static_cast<int64_t>(inner_sum);
            }

            iter_total += local;
        }

        final_total = iter_total;
    }

    return final_total;
}