#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    const auto d_begin = thrust::device_pointer_cast<const float>(input);
    const auto d_end = d_begin + n;
    for (int iter = 0; iter < iters; ++iter) {
        const float sum = thrust::reduce(d_begin, d_end, 0.0f, thrust::plus<float>());
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}