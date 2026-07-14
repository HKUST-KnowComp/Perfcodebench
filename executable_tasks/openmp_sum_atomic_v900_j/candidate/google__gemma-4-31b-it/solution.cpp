#include "interface.h"

#include <algorithm>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    const size_t n = input.size();
    const uint32_t* data = input.data();
    uint64_t total_sum = 0;

    // Set threads once to avoid repeated overhead
    int threads = std::max(1, std::min(8, omp_get_max_threads()));
    omp_set_num_threads(threads);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        #pragma omp parallel for reduction(+:local_sum) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            local_sum += static_cast<uint64_t>(data[i]);
        }
        total_sum = local_sum;
    }

    return total_sum;
}