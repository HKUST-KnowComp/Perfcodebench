#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemsetAsync(output, 0, sizeof(int), 0);
        }
        return;
    }

    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    
    // Determine temporary device storage requirements
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage
#if CUDART_VERSION >= 11020
    cudaMallocAsync(&d_temp_storage, temp_storage_bytes, 0);
#else
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
#endif
    
    // Run the reduction for the specified number of iterations
    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
    
    // Free temporary storage
#if CUDART_VERSION >= 11020
    cudaFreeAsync(d_temp_storage, 0);
#else
    cudaFree(d_temp_storage);
#endif
}
