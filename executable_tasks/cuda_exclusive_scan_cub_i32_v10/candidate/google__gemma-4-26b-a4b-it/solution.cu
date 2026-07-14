#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to cub::DeviceExclusiveScan to determine the amount of temporary storage needed
    cub::DeviceExclusiveScan::TempStorage<int>::ArrayList temp_storage;
    // Note: For simplicity and robustness in a single-function call, we use the generic TempStorage approach.
    // However, to be most efficient, we'll use the standard CUB pattern.
    
    // We need to find the size first.
    cub::DeviceExclusiveScan::Scan(nullptr, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceExclusiveScan::Scan(d_temp_storage, input, output, n);
    }

    cudaFree(d_temp_storage);
}