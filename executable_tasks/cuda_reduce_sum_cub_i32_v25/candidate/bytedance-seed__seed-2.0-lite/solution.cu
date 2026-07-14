#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Allocate CUB temporary storage once to avoid repeated allocation overhead
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // Query required temporary storage size for the reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run reduction for all requested iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up allocated temporary memory
    cudaFree(d_temp_storage);
}