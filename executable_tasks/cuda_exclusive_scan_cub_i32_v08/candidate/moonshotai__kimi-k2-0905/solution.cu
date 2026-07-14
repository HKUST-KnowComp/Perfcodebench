#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    // Determine temporary storage size (once)
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes,
                                  input, output, n);

    // Allocate temporary storage
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run the scan repeatedly
    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes,
                                      input, output, n);
    }

    // Free temporary storage
    cudaFree(d_temp_storage);
}