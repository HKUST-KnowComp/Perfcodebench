#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    
    // Query required temporary storage size
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    
    // Allocate temporary storage once
    cudaMalloc(&d_temp, temp_bytes);
    
    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }
    
    cudaFree(d_temp);
}