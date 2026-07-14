#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Determine temporary storage size for CUB reduction
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage once
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Repeated reductions (no memset needed; CUB writes the sum directly)
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Cleanup
    cudaFree(d_temp_storage);
}
