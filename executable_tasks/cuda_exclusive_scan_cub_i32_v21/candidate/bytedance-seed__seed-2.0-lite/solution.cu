#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    // Query and allocate CUB temporary storage once to avoid repeated overhead
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cudaError_t err = cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
    if (err != cudaSuccess) return;
    
    err = cudaMalloc(&d_temp, temp_bytes);
    if (err != cudaSuccess) return;

    // Run scan for all required iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
    }

    // Cleanup temporary device memory
    cudaFree(d_temp);
}