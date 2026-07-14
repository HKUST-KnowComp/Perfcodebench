#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Query required CUB temporary storage size
    void* d_temp = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, output, n);
    
    // Allocate temporary storage once for all iterations
    cudaMalloc(&d_temp, temp_storage_bytes);

    // Execute reduction across all iterations
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cub::DeviceReduce::Sum(d_temp, temp_storage_bytes, input, output, n);
    }

    // Cleanup temporary resources
    cudaFree(d_temp);
}