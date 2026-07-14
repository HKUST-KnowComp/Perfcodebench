#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Query and allocate CUB temporary storage for reduction
    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_bytes);

    // Run repeated reductions per task requirements
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }

    // Cleanup temporary resources
    cudaFree(d_temp);
}