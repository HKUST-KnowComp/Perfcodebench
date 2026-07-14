// Optimized CUDA exclusive scan using CUB
// Author: Performance optimization

#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static int* d_temp = nullptr;
    static size_t temp_storage_bytes = 0;
    static int prev_n = 0;

    // Allocate temporary storage if n changed or first call
    if (n != prev_n) {
        if (d_temp) {
            cudaFree(d_temp);
        }
        // Determine temporary device storage requirements
        cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp, temp_storage_bytes);
        prev_n = n;
    }

    // Perform the exclusive scan 'iters' times, reusing precomputed allocation
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
    }
}
