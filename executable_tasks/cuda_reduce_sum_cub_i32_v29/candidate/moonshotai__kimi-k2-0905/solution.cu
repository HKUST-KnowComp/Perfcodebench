#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include <cstdint>

namespace {

__global__ void copy_result(int* dst, const int* src) { *dst = *src; }

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // device buffer for the reduction result
    int* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(int));

    // determine temporary storage size
    size_t temp_bytes = 0;
    cub::DeviceReduce::Sum(nullptr, temp_bytes, input, d_out, n);
    void* d_temp = nullptr;
    cudaMalloc(&d_temp, temp_bytes);

    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp, temp_bytes, input, d_out, n);
        // write result to user-supplied pointer
        copy_result<<<1,1>>>(output, d_out);
    }

    cudaFree(d_temp);
    cudaFree(d_out);
}