#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Static persistent temporary storage for CUB reductions,
    // grown as needed but never freed for performance.
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;

    // Query required temporary storage bytes
    size_t tmp_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, tmp_bytes, input, output, n);

    // Allocate or reallocate if more space is needed
    if (tmp_bytes > temp_storage_bytes) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
        }
        cudaMalloc(&d_temp_storage, tmp_bytes);
        temp_storage_bytes = tmp_bytes;
    }

    // Run the reduction the requested number of iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    // No explicit synchronisation required; the harness will call cudaDeviceSynchronize
}