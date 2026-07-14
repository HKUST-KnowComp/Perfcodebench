#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int iter = 0; iter < iters; ++iter) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    int64_t* d_sum = nullptr;
    cudaMalloc(&d_sum, sizeof(int64_t));

    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_sum, n);
    cudaMalloc(&d_temp, temp_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_sum, n);
        cudaMemcpy(output, d_sum, sizeof(int), cudaMemcpyDeviceToDevice);
    }

    cudaFree(d_temp);
    cudaFree(d_sum);
}