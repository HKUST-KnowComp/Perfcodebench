#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    static int* d_temp_storage = nullptr;
    static size_t temp_storage_bytes = 0;

    // Query required temporary storage size (using a null pointer)
    size_t new_temp_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, new_temp_bytes, input, output, n);

    // Grow the temporary buffer if necessary
    if (new_temp_bytes > temp_storage_bytes) {
        if (d_temp_storage) {
            cudaFree(d_temp_storage);
        }
        cudaMalloc(&d_temp_storage, new_temp_bytes);
        temp_storage_bytes = new_temp_bytes;
    }

    // Perform the reduction iters times; output is written directly (no memset needed)
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
}