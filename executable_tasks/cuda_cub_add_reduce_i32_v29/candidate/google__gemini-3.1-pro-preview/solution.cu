#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

/**
 * Optimized CUDA int32 sum reduction using the CUB library.
 * CUB's DeviceReduce::Sum is highly optimized for various GPU architectures,
 * utilizing efficient memory access patterns and warp-level primitives.
 */
void add_reduce_i32(const int* input, int* output, int n, int iters) {
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

    // Allocate temporary storage once to reuse across iterations
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // CUB DeviceReduce::Sum computes the sum and writes it to the output pointer.
        // It does not require the output to be zero-initialized as it overwrites the value.
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up temporary storage
    cudaFree(d_temp_storage);
}