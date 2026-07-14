#include "interface.h"
#include <cub/cub.hpp>
#include <cuda_runtime.h>

/**
 * Optimized CUDA int32 reduction using CUB.
 * CUB's DeviceReduce::Sum is highly tuned for various GPU architectures and input sizes,
 * utilizing efficient block-level reductions and global synchronization strategies.
 */

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB's DeviceReduce::Sum handles the initialization of the output internally
        // if we provide it as the destination, but it actually overwrites it.
        // However, the task requires the sum of the array. 
        // cub::DeviceReduce::Sum computes the sum and stores it in output.
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}