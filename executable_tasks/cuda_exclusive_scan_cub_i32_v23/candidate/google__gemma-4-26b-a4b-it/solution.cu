#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.hpp>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to determine the required temporary storage size
    cub::DeviceExclusiveScan(input, output, n, d_temp_storage, temp_storage_bytes);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceExclusiveScan(input, output, n, d_temp_storage, temp_storage_bytes);
    }

    cudaFree(d_temp_storage);
}