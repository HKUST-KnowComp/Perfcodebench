#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

/**
 * Performs an exclusive prefix sum (scan) on an int32 array.
 * Uses the CUB library's DeviceScan for optimal performance.
 */
void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Initial call to determine the required size of the temporary buffer
    cub::DeviceScan::ExclusiveSum(
        nullptr, 
        temp_storage_bytes, 
        input, 
        output, 
        n
    );

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Execute the scan 'iters' times as required by the benchmark harness
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(
            d_temp_storage, 
            temp_storage_bytes, 
            input, 
            output, 
            n
        );
    }

    // Clean up temporary storage
    cudaFree(d_temp_storage);
}