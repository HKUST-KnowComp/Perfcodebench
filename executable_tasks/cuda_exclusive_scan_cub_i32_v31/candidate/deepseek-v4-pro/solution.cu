#include "interface.h"
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    // Determine temporary storage size required by CUB
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage on device
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Perform exclusive scan iters times
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Free temporary storage
    cudaFree(d_temp_storage);
}