#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Calculate required temporary storage size for CUB scan
    void* d_temp = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage once, reused across all iterations
    cudaMalloc(&d_temp, temp_storage_bytes);

    // Run scan for all requested iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
    }

    // Cleanup allocated temporary memory
    cudaFree(d_temp);
}