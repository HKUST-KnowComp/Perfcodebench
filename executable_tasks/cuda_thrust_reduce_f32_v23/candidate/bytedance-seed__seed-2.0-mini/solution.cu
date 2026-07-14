#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return 0.0f;
    }
    float* d_result = nullptr;
    cudaMalloc(&d_result, sizeof(float));
    for (int iter = 0; iter < iters; ++iter) {
        thrust::device_ptr<float> dev_out(d_result);
        *dev_out = thrust::reduce(thrust::device, input, input + n, 0.0f);
    }
    float actual = 0.0f;
    cudaMemcpy(&actual, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_result);
    return actual;
}