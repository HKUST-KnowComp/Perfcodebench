#include "interface.h"

#include <cuda_runtime.h>
#include <cub/device/device_scan.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    // Determine temporary device storage requirements
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run the scan the requested number of times
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Free temporary storage
    cudaFree(d_temp_storage);
}
