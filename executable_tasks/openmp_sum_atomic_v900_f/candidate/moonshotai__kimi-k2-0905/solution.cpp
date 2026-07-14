#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

int num_threads() {
    return std::max(1, std::min(8, omp_get_max_threads()));
}

}  // namespace

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    const int threads = num_threads();
    omp_set_num_threads(threads);

    const std::size_t n = input.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::vector<uint64_t> partial(threads, 0ULL);

#pragma omp parallel num_threads(threads)
        {
            const int tid     = omp_get_thread_num();
            const std::size_t chunk = (n + threads - 1) / threads;
            const std::size_t start = tid * chunk;
            const std::size_t end  = std::min(start + chunk, n);

            uint64_t local = 0;
            for (std::size_t i = start; i < end; ++i) {
                local += static_cast<uint64_t>(input[i]);
            }
            partial[tid] = local;
        }

        uint64_t sum = 0;
        for (int t = 0; t < threads; ++t) sum += partial[t];
        total = sum;
    }
    return total;
}