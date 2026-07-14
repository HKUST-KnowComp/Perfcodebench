#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveScan(nullptr, temp_storage_bytes, input, output, cub::Sum(), n);
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveScan(d_temp_storage, temp_storage_bytes, input, output, cub::Sum(), n);
    }
    cudaFree(d_temp_storage);
}
