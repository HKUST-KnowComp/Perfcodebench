#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

/**
 * Optimized CUDA int32 reduction using CUB.
 * CUB's DeviceReduce::Sum is highly optimized for various GPU architectures,
 * utilizing efficient tree-based reductions and vectorized memory access.
 */
void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    // Determine temporary device storage requirements for CUB
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage once to avoid overhead inside the loop
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB's DeviceReduce::Sum writes the result directly to the output pointer.
        // It does not require the output to be zeroed beforehand as it performs an assignment.
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up temporary storage
    cudaFree(d_temp_storage);
}