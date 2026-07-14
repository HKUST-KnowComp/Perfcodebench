#include "interface.h"
#include <omp.h>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    if (input.empty()) return 0;

    // Determine optimal thread count once
    int max_threads = omp_get_max_threads();
    int num_threads = std::max(1, std::min(8, max_threads));
    omp_set_num_threads(num_threads);

    uint64_t total_sum = 0;
    const uint32_t* data = input.data();
    size_t n = input.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        // Use reduction clause to avoid atomic contention on a single shared variable.
        // This allows each thread to work on a private accumulator.
        #pragma omp parallel for reduction(+:iter_sum) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            iter_sum += static_cast<uint64_t>(data[i]);
        }
        total_sum = iter_sum;
    }

    return total_sum;
}