#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Query required temporary storage size for CUB's reduction
    void* temp_storage = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(temp_storage, temp_bytes, input, output, n);

    // Allocate temporary storage once, reused across all iterations
    cudaMalloc(&temp_storage, temp_bytes);

    // Execute reduction for all requested iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(temp_storage, temp_bytes, input, output, n);
    }

    // Clean up allocated temporary memory
    cudaFree(temp_storage);
}