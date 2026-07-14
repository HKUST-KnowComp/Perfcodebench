#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static char* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    static int cached_n = 0;

    if (n <= 0) {
        return;
    }

    if (n != cached_n) {
        if (d_temp_storage != nullptr) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
        }

        cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

        const cudaError_t alloc_err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
        if (alloc_err != cudaSuccess) {
            cached_n = 0;
            return;
        }

        cached_n = n;
    }

    for (int iter = 0; iter < iters; ++iter) {
        const cudaError_t scan_err = cub::DeviceScan::ExclusiveSum(
            d_temp_storage, temp_storage_bytes, input, output, n);
        if (scan_err != cudaSuccess) {
            return;
        }
    }
}