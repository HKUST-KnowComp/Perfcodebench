#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Allocate CUB temporary storage for reductions
    void* d_temp = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Query required temporary storage size once
    cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_storage_bytes);
    
    // Run reduction for all benchmark iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, output, n);
    }
    
    // Cleanup temporary device memory
    cudaFree(d_temp);
}