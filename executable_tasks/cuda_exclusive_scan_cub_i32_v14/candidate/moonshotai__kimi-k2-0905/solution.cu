#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
    void* d_temp = nullptr;
    cudaMalloc(&d_temp, temp_storage_bytes);
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
    }
    cudaFree(d_temp);
}