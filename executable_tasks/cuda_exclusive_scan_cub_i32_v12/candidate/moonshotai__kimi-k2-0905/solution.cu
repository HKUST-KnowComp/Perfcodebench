#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

namespace {

__global__ void cub_exclusive_scan_kernel(const int* input,
                                          int*       output,
                                          int        n) {
    using PolicyT = cub::DeviceScanPolicy<int>::ExclusiveScan;
    
    // Allocate temporary storage in global memory via a single runtime allocation.
    // We use a fixed-size buffer sized for the largest possible n (INT_MAX) with
    // CUB's worst-case bytes requirement.  A single allocation is reused for all
    // iterations to avoid per-launch overhead.
    const size_t temp_storage_bytes = cub::DeviceScan::ExclusiveSum(nullptr, input, output, n);
    __shared__ uint8_t temp_storage[1];  // placeholder to force cub to use its internal path

    cub::DeviceScan::ExclusiveSum(input, output, n);
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    // Determine temporary storage size once and allocate once.
    size_t temp_storage_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

    uint8_t* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes,
                                      input, output, n);
    }

    cudaFree(d_temp_storage);
}