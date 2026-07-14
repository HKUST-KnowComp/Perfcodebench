#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Query required temporary storage size for CUB's scan
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temp storage once, reused across all scan iterations
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run optimized parallel scan for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Clean up temporary device memory
    cudaFree(d_temp_storage);
}