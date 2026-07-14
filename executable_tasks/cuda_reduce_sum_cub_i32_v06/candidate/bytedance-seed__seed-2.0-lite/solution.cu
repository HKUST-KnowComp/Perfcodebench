#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Get required size for CUB's temporary reduction storage
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage once before all iterations
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run optimized reduction for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Cleanup allocated temporary memory
    cudaFree(d_temp_storage);
}