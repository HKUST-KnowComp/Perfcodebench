#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Query required temporary storage size
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Execute reduction for each iteration
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Clean up
    cudaFree(d_temp_storage);
}