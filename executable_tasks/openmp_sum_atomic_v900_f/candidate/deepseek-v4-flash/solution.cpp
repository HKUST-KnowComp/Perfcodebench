#include "interface.h"
#include <cstdint>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    uint64_t total = 0;
    const std::size_t n = input.size();
    // Compute the sum of all elements once using an efficient parallel reduction.
#pragma omp parallel for reduction(+:total)
    for (std::size_t i = 0; i < n; ++i) {
        total += input[i];
    }
    // Repeated summation is equivalent to multiplying by iters.
    return total * iters;
}