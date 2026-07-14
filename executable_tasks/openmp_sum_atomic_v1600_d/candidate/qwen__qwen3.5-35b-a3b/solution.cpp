#include "interface.h"
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
#pragma omp parallel for reduction(+:iter_sum)
        for (std::size_t i = 0; i < input.size(); ++i) {
            iter_sum += static_cast<uint64_t>(input[i]);
        }
        total_sum += iter_sum;
    }
    return total_sum;
}