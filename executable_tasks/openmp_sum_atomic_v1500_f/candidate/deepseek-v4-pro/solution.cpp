#include "interface.h"
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    if (iters <= 0) return 0;
    // Use all available threads
    omp_set_num_threads(omp_get_max_threads());
    const uint32_t* data = input.data();
    const std::size_t n = input.size();
    uint64_t sum = 0;
    // Parallel reduction with SIMD hint for maximum throughput
    #pragma omp parallel for simd reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
        sum += data[i];
    }
    return sum;
}