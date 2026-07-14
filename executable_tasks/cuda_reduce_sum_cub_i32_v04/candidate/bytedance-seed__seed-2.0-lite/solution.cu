#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Query required temporary storage size for CUB reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Run reduction for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Cleanup allocated temporary memory
    cudaFree(d_temp_storage);
}