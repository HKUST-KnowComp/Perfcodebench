#include "interface.h"
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    uint64_t sum = 0;
    const std::size_t n = input.size();
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < n; ++i) {
        sum += static_cast<uint64_t>(input[i]) * iters;
    }
    return sum;
}
