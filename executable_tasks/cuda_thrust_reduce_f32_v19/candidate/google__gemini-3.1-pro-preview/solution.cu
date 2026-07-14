#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

/**
 * Performs a parallel reduction of a float array using CUB.
 * CUB is highly optimized for various GPU architectures and provides
 * significantly better performance than a manual sequential or basic tree reduction.
 */
float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    // Determine temporary device storage requirements for CUB reduction
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, d_out, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Execute the reduction 'iters' times as required by the benchmark harness.
    // Using cub::DeviceReduce::Sum with a device output pointer avoids 
    // implicit host-device synchronization inside the loop.
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_out, n);
    }

    // Copy the final result back to the host
    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    // Cleanup
    cudaFree(d_out);
    cudaFree(d_temp_storage);

    return result;
}