#include "interface.h"
#include <cuda_runtime.h>
#include <cub/device/device_reduce.cuh>

/**
 * Optimized CUDA int32 sum reduction using CUB.
 * CUB's DeviceReduce::Sum is highly optimized for various GPU architectures,
 * utilizing techniques like vectorized loads, warp-level primitives, and 
 * efficient multi-pass or single-pass (one-scan) reduction strategies.
 */
void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run the reduction for the specified number of iterations
    for (int iter = 0; iter < iters; ++iter) {
        // CUB DeviceReduce::Sum writes the result directly to the output pointer.
        // It does not require a manual cudaMemset(output, 0, ...) because it 
        // overwrites the target memory with the final reduction result.
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up temporary storage
    cudaFree(d_temp_storage);
}