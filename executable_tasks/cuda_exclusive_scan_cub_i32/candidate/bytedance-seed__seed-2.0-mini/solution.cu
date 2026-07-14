#include "interface.h"
#include <cuda_runtime.h>
#include <cub/device/device_scan.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        return;
    }

    static int current_n = 0;
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;

    if (n != current_n) {
        if (d_temp_storage != nullptr) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
        }
        cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
        current_n = n;
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
}