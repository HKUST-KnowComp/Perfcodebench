#include "interface.h"
#include <cuda_runtime.h>
#include <cub/device/device_reduce.cuh>

namespace {

__global__ void cast_kernel(const long long* in, int* out) {
    *out = static_cast<int>(*in);
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (iters <= 0) {
        return;
    }

    long long* d_temp_sum = nullptr;
    cudaMalloc(&d_temp_sum, sizeof(long long));

    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_temp_sum, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);

    for (int iter = 0; iter < iters; ++iter) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_temp_sum, n);
        cast_kernel<<<1, 1>>>(d_temp_sum, output);
    }

    cudaFree(d_temp_storage);
    cudaFree(d_temp_sum);
}
