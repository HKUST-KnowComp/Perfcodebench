#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <cuda_runtime.h>

float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    thrust::device_ptr<const float> d_input(input);
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    cudaEventRecord(start);
    for (int iter = 0; iter < iters; ++iter) {
        *d_out = thrust::reduce(d_input, d_input + n);
    }
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    return actual;
}