#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    
    // Query CUB for required temporary storage size
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    
    // Allocate temporary storage once, reused across all iterations
    cudaMalloc(&d_temp, temp_bytes);
    
    // Run optimized reduction for all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }
    
    // Clean up allocated temporary storage
    cudaFree(d_temp);
}