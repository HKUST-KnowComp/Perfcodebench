#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int i = 0; i < iters; ++i) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    void* d_temp = nullptr;
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    cudaMalloc(&d_temp, temp_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
    }

    cudaFree(d_temp);
}