#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Determine required temporary storage size
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_bytes, input, output, n);
    // Allocate temporary storage once
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_bytes);
    // Run the reduction 'iters' times
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_bytes, input, output, n);
    }
    cudaFree(d_temp_storage);
}
