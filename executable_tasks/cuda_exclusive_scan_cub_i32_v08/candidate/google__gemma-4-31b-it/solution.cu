#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary storage requirements for CUB
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveScan(nullptr, 0, input, output, n, d_temp_storage, temp_storage_bytes);

    if (temp_storage_bytes > 0) {
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
    }

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveScan(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    if (d_temp_storage) {
        cudaFree(d_temp_storage);
    }
}