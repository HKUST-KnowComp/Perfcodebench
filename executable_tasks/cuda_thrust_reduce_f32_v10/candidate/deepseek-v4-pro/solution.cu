#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    thrust::device_ptr<const float> d_input(input);
    float* d_result = nullptr;
    cudaMalloc(&d_result, sizeof(float));

    thrust::device_ptr<float> d_out(d_result);

    for (int iter = 0; iter < iters; ++iter) {
        thrust::reduce(thrust::device, d_input, d_input + n, d_out);
    }

    float result = 0.0f;
    cudaMemcpy(&result, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_result);

    return result;
}
