#include "interface.h"
#include <omp.h>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    // Determine optimal thread count (clamped to 8 as per baseline logic)
    int threads = std::max(1, std::min(8, omp_get_max_threads()));
    omp_set_num_threads(threads);

    uint64_t total_sum = 0;
    const size_t n = input.size();
    const uint32_t* data = input.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        // Use OpenMP reduction to avoid atomic contention.
        // Each thread computes a local sum, which is combined at the end.
        #pragma omp parallel for reduction(+:iter_sum) schedule(static)
        for (std::size_t i = 0; i < n; ++i) {
            iter_sum += static_cast<uint64_t>(data[i]);
        }
        total_sum = iter_sum;
    }

    return total_sum;
}