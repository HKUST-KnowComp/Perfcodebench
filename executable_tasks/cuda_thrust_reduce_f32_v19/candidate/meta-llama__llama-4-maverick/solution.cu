#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <cuda_runtime.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_input = nullptr;
    cudaMalloc((void**)&d_input, n * sizeof(float));
    cudaMemcpy(d_input, input, n * sizeof(float), cudaMemcpyHostToDevice);

    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(thrust::device, d_input, d_input + n);
    }

    cudaFree(d_input);
    return actual;
}