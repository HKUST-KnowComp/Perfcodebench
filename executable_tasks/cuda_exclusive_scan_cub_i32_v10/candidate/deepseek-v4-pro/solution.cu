#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        // No work to do, but the harness expects the loop to run iters times.
        // Launching a no-op kernel or skips would change the timing pattern;
        // we can simply call cub with zero items to match baseline behavior.
        return;
    }

    // Query temporary storage size
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage once
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Run the scan iters times to match the required interface
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // Synchronize the default stream so that harness timing is correct
    cudaDeviceSynchronize();

    // Free temporary storage
    cudaFree(d_temp_storage);
}
