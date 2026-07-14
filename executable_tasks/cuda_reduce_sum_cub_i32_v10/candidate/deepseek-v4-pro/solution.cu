#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Determine required temporary storage size
    size_t temp_bytes = 0;
    void* d_temp = nullptr;
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    
    // Allocate temporary storage once
    cudaMalloc(&d_temp, temp_bytes);
    
    // Repeated reductions as required by the harness
    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }
    
    cudaFree(d_temp);
}
