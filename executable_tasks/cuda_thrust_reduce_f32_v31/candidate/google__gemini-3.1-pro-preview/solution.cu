#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Determine temporary device storage requirements
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_out, n);
    
    // Allocate temporary storage
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    
    // Run reduction for the specified number of iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_out, n);
    }
    
    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    
    cudaFree(d_temp_storage);
    cudaFree(d_out);
    
    return actual;
}
