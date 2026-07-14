#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to find the required size of temporary storage
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB's DeviceReduce::Sum handles the reduction efficiently
        // Note: output must be initialized to 0 if we were doing multiple passes on the same output,
        // but DeviceReduce::Sum overwrites the output with the result.
        cub::DeviceReduce::Sum(d_temp_storage, n, input, output);
    }

    cudaFree(d_temp_storage);
}