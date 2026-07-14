#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    size_t temp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);

    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}