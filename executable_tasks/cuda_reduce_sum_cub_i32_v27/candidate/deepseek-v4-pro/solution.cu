#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Handle zero-size array: just zero the output each iteration
    if (n == 0) {
        for (int i = 0; i < iters; ++i) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    // Determine temporary device storage requirements for CUB reduction
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

    // Allocate temporary storage once before the iteration loop
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    // Perform the reduction multiple times; each call writes the result directly to output
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }

    cudaFree(d_temp_storage);
}
