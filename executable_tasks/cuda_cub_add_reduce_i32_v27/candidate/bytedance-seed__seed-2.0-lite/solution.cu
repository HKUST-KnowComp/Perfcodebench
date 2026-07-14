#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    // Query required temporary storage size for CUB reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    // Allocate temp storage once for all iterations
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run optimized reduction for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Cleanup temporary storage
    cudaFree(d_temp_storage);
}