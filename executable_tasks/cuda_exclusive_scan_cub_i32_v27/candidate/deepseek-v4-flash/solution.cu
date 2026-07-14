#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;
    static size_t prev_n = 0;

    if (n != prev_n) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
            d_temp_storage = nullptr;
            temp_storage_bytes = 0;
        }
        cub::DeviceScan::ExclusiveScan(nullptr, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
        prev_n = n;
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveScan(d_temp_storage, temp_storage_bytes, input, output, n);
        cudaDeviceSynchronize();
    }
}