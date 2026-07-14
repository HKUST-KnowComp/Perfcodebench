#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Determine temporary device storage requirements
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage
    if (temp_storage_bytes > 0) {
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
    }
    
    // Execute exclusive scan for the requested number of iterations
    for (int i = 0; i < iters; ++i) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    if (d_temp_storage) {
        cudaFree(d_temp_storage);
    }
}