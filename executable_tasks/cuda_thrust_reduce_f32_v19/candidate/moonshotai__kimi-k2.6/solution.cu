#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return 0.0f;
    }

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    void* d_temp_storage = nullptr;
    size_t temp_storage_bytes = 0;
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_out, n);
    if (temp_storage_bytes > 0) {
        cudaMalloc(&d_temp_storage, temp_storage_bytes);
    }

    for (int i = 0; i < iters; ++i) {
        cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_out, n);
    }

    float result = 0.0f;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_out);
    if (d_temp_storage) {
        cudaFree(d_temp_storage);
    }

    return result;
}
