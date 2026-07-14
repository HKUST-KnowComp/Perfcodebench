#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Allocate one temporary device buffer for CUB
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_bytes);

    // Warm-up / correctness run
    cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);

    // Timed iterations
    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
    }

    cudaFree(d_temp);
}