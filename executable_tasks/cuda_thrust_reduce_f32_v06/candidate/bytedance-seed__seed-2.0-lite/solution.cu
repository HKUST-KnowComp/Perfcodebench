#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;

    // Wrap raw device pointer for Thrust compatibility
    thrust::device_ptr<const float> d_input(input);

    for (int iter = 0; iter < iters; ++iter) {
        // Thrust's optimized parallel sum reduction
        float sum = thrust::reduce(d_input, d_input + n, 0.0f);
        // Match baseline's memory access pattern by updating device output
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }

    // Preserve original host copy and cleanup workflow
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}