#include "interface.h"
#include <omp.h>

namespace {

int num_threads() {
    int max = omp_get_max_threads();
    if (max > 8) max = 8;
    if (max < 1) max = 1;
    return max;
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    omp_set_num_threads(num_threads());
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        #pragma omp parallel for reduction(+:sum) schedule(static)
        for (std::size_t i = 0; i < input.size(); ++i) {
            sum += input[i];
        }
    }
    return sum;
}