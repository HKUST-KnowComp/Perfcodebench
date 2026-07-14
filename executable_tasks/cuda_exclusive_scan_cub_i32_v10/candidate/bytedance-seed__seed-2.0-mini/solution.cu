#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }

    // Calculate required temporary storage size once per call
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary device storage
    void* temp_storage = nullptr;
    const cudaError_t alloc_err = cudaMalloc(&temp_storage, temp_storage_bytes);
    if (alloc_err != cudaSuccess) {
        return;
    }

    // Execute exclusive scan for each requested iteration
    for (int i = 0; i < iters; ++i) {
        const cudaError_t scan_err = cub::DeviceScan::ExclusiveSum(
            temp_storage, temp_storage_bytes, input, output, n);
        if (scan_err != cudaSuccess) {
            break;
        }
    }

    // Clean up temporary storage
    cudaFree(temp_storage);
}