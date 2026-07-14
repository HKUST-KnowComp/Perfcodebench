#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>
#include <thrust/system/cuda/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    cudaStream_t stream;
    cudaStreamCreate(&stream);
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    thrust::device_ptr<const float> dev_input = thrust::device_pointer_cast(input);
    thrust::device_ptr<float> dev_output = thrust::device_pointer_cast(d_out);
    auto policy = thrust::cuda::par.on(stream);
    for (int iter = 0; iter < iters; ++iter) {
        thrust::reduce(policy, dev_input, dev_input + n, dev_output);
    }
    float actual = 0.0f;
    cudaMemcpyAsync(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost, stream);
    cudaStreamSynchronize(stream);
    cudaStreamDestroy(stream);
    cudaFree(d_out);
    return actual;
}
