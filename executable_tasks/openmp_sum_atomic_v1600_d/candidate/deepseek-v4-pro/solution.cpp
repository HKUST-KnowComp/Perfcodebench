#include "interface.h"

#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    omp_set_dynamic(0);
    omp_set_num_threads(omp_get_max_threads());
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
#pragma omp parallel for reduction(+ : sum) schedule(static)
        for (std::size_t i = 0; i < input.size(); ++i) {
            sum += static_cast<uint64_t>(input[i]);
        }
    }
    return sum;
}