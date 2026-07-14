#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    static bool allocated = false;
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;

    if (!allocated) {
        cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
        allocated = true;
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
}
