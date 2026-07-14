#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Allocate CUB's required temporary reduction storage
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    // Query required temporary storage size for sum reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Execute optimized CUB sum reduction
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Cleanup temporary device storage
    cudaFree(d_temp_storage);
}