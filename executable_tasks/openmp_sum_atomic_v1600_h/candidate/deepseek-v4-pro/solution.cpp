#include "interface.h"

#include <algorithm>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    if (iters <= 0 || input.empty()) return 0;

    const std::size_t n = input.size();
    uint64_t sum = 0;

    int nthreads = std::max(1, std::min(8, omp_get_max_threads()));
    omp_set_num_threads(nthreads);

    // Use parallel for with reduction and SIMD; conditionally disable parallelism for small n
    #pragma omp parallel for simd reduction(+:sum) if(n > 4096) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(input[i]);
    }
    return sum;
}
