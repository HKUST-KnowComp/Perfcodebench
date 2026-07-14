#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    // Set thread count once to avoid overhead
    int num_threads = std::max(1, std::min(8, omp_get_max_threads()));
    omp_set_num_threads(num_threads);

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        // Use reduction instead of atomic to avoid serialization/lock contention
        #pragma omp parallel for reduction(+:sum)
        for (std::size_t i = 0; i < input.size(); ++i) {
            sum += static_cast<uint64_t>(input[i]);
        }
    }
    return sum;
}