#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        float sum = thrust::reduce(input, input + n);
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}