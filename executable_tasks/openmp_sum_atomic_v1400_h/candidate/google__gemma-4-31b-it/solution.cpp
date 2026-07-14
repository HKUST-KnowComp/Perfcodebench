#include "interface.h"
#include <omp.h>
#include <vector>
#include <cstdint>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    const size_t n = input.size();
    const uint32_t* data = input.data();
    uint64_t total_sum = 0;

    // Set threads once outside the loop to reduce overhead
    int threads = omp_get_max_threads();
    omp_set_num_threads(threads);

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        #pragma omp parallel for reduction(+:current_sum) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            current_sum += static_cast<uint64_t>(data[i]);
        }
        total_sum = current_sum;
    }

    return total_sum;
}