#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.hpp>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to determine the amount of temporary storage needed
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB's DeviceReduce::Sum handles the reduction efficiently
        // Note: The output must be initialized to 0 if the kernel doesn't overwrite it, 
        // but DeviceReduce::Sum writes the result directly to the output pointer.
        // However, to ensure correctness across iterations if the harness expects a clean state,
        // we rely on the fact that DeviceReduce::Sum performs a full reduction.
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}