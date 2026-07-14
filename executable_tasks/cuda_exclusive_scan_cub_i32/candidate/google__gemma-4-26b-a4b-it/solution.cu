#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.hpp>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine temporary device storage requirements
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;

    // First call to determine the amount of temporary storage needed
    cub::DeviceScan::ExclusiveScan(
        d_temp_storage, 
        temp_storage_bytes, 
        input, 
        output, 
        n
    );

    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        // Perform the actual scan
        cub::DeviceScan::ExclusiveScan(
            d_temp_storage, 
            temp_storage_bytes, 
            input, 
            output, 
            n
        );
    }

    // Clean up
    cudaFree(d_temp_storage);
}