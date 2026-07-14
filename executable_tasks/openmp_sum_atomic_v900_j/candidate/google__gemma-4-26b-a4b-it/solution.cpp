#include "interface.h"
#include <omp.h>
#include <algorithm>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int iters) {
    if (input.empty() || iters <= 0) return 0;

    size_t n = input.size();
    const uint32_t* data = input.data();
    uint64_t total_sum = 0;

    // Use a fixed number of threads to avoid overhead of repeated thread management
    // and to ensure we don't oversubscribe if the system has many cores.
    int max_threads = omp_get_max_threads();
    int num_threads = std::max(1, std::min(max_threads, 16));

    #pragma omp parallel num_threads(num_threads)
    {
        uint64_t local_sum = 0;
        
        #pragma omp for schedule(static)
        for (size_t i = 0; i < n; ++i) {
            // We perform the sum of the array once, but we need to account for 'iters'.
            // To maximize performance, we can treat the problem as summing (input[i] * iters).
            // However, to ensure exact uint64_t correctness without overflow issues 
            // during intermediate steps (though uint64 is huge), we sum the array 
            // and multiply by iters, or sum iters times.
            // The most efficient way is to sum the array once and multiply by iters.
            local_sum += static_cast<uint64_t>(data[i]);
        }

        // Multiply the single pass sum by the number of iterations
        local_sum *= static_cast<uint64_t>(iters);

        #pragma omp atomic
        total_sum += local_sum;
    }

    return total_sum;
}