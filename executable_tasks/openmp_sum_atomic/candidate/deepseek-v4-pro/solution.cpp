#include "interface.h"

#include <cstdint>
#include <omp.h>

namespace {

int num_threads() {
    return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    if (iters <= 0) return 0;
    omp_set_num_threads(num_threads());
    uint64_t sum = 0;
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (std::size_t i = 0; i < input.size(); ++i) {
        sum += static_cast<uint64_t>(input[i]);
    }
    return sum;
}
