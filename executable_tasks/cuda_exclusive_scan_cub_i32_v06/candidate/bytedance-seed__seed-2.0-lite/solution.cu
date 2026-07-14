#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    // Query required temporary storage size for CUB's parallel scan
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);

    // Allocate temporary storage once before the iteration loop
    cudaMalloc(&d_temp, temp_bytes);

    // Run scan for all requested iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
    }

    // Clean up allocated temporary memory
    cudaFree(d_temp);
}