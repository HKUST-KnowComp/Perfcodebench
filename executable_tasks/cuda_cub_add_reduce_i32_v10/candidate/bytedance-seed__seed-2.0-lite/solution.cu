#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Preallocate CUB temporary storage once to avoid per-iteration allocation overhead
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    // Query required temporary buffer size for sum reduction
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Launch optimized CUB sum reduction, which uses shared memory, warp shuffles, and minimal atomic operations
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }

    // Cleanup temporary device memory
    cudaFree(d_temp);
}