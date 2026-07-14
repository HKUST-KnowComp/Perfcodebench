#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Query and allocate CUB temporary storage once to avoid repeated overhead
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run optimized reduction for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Cleanup temporary resources
    cudaFree(d_temp_storage);
}