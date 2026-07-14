#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Static variables to cache temporary storage for CUB reduction
    static void* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;

    // Determine required temporary storage size
    size_t new_temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, new_temp_storage_bytes, input, output, n);
    if (new_temp_storage_bytes > temp_storage_bytes) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
        }
        cudaMalloc(&d_temp_storage, new_temp_storage_bytes);
        temp_storage_bytes = new_temp_storage_bytes;
    }

    // Run the reduction multiple times as requested by the harness
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
}
