#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    long long* d_sum_dev = nullptr;

    // Query required temporary storage size for CUB reduction
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_sum_dev, n);
    cudaMalloc(&d_temp, temp_bytes);
    cudaMalloc(&d_sum_dev, sizeof(long long));

    for (int iter = 0; iter < iters; ++iter) {
        // Compute exact sum in 64-bit to avoid overflow during reduction
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_sum_dev, n);
        // Cast to int32 to meet correctness requirement, write to output
        cudaMemcpy(output, d_sum_dev, sizeof(int), cudaMemcpyDeviceToDevice);
    }

    // Cleanup temporary allocations
    cudaFree(d_temp);
    cudaFree(d_sum_dev);
}