#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Handle empty input
    if (n <= 0) {
        int zero = 0;
        cudaMemcpy(output, &zero, sizeof(int), cudaMemcpyHostToDevice);
        return;
    }

    // Determine temporary storage size
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary device memory
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Repeat reduction the requested number of times
    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up
    cudaFree(d_temp_storage);
}
