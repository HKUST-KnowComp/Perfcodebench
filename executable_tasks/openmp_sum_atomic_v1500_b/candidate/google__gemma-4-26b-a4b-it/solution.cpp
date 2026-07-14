#include "interface.h"
#include <omp.h>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    const size_t n = input.size();
    const uint32_t* data = input.data();
    uint64_t total_sum = 0;

    // We perform the iterations outside the parallel region to avoid the overhead 
    // of repeatedly spawning/joining threads, which is significant for large counts.
    // However, to strictly follow the logic of the original (summing the whole buffer 'iters' times),
    // we can accumulate the result of each iteration.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        #pragma omp parallel for reduction(+:iter_sum) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            iter_sum += static_cast<uint64_t>(data[i]);
        }
        total_sum += iter_sum;
    }

    return total_sum;
}