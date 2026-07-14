#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to determine the amount of temporary storage needed
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB's DeviceReduce::Sum is highly optimized for various architectures
        // and handles the reduction tree and memory coalescing internally.
        cub::DeviceReduce::Sum(d_temp_storage, n, input, output, n);
    }

    cudaFree(d_temp_storage);
}