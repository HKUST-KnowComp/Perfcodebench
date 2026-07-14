#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Determine temporary storage requirements
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage once for all iterations
    void* d_temp_storage = nullptr;
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset output before each reduction (matches baseline behavior)
        cudaMemset(output, 0, sizeof(int));
        
        // Perform reduction (CUB handles multiple kernel launches internally)
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    cudaFree(d_temp_storage);
}
