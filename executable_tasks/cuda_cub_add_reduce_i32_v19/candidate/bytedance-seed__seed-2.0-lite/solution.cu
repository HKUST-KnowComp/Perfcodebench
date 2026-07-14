#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Query required temporary storage size for CUB reduction
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    
    // Allocate persistent temporary storage for all iterations
    cudaMalloc(&d_temp, temp_bytes);
    
    // Run repeated reductions as requested
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }
    
    // Cleanup allocated temporary memory
    cudaFree(d_temp);
}