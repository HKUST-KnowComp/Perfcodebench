#include "interface.h"
#include <cub/device/device_scan.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run the scan for the specified number of iterations
    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Cleanup
    cudaFree(d_temp_storage);
}