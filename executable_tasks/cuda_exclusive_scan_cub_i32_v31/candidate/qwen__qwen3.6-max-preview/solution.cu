#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    (void)cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        (void)cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}
