#include "interface.h"

#include <cuda_runtime.h>
#include <cub/device/device_scan.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Cache temporary storage across calls to avoid repeated allocations.
    static size_t temp_bytes = 0;
    static void* d_temp_storage = nullptr;

    // Determine required temporary storage for the current n.
    size_t required_temp_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, required_temp_bytes, input, output, n);
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) return;   // propagate error silently

    // Reallocate if cached storage is too small.
    if (temp_bytes < required_temp_bytes) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
        }
        cudaMalloc(&d_temp_storage, required_temp_bytes);
        temp_bytes = required_temp_bytes;
    }

    // Run the exclusive scan `iters` times.
    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_bytes, input, output, n);
    }
}