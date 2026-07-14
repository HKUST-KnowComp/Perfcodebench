#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    const thrust::device_ptr<const float> dev_input(input);

    for (int iter = 0; iter < iters; ++iter) {
        // Execute Thrust's optimized device-side parallel reduction
        float sum = thrust::reduce(dev_input, dev_input + n, 0.0f);
        // Match baseline's per-iteration device output write pattern
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }

    // Copy final result back to host and clean up, matching baseline lifecycle
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}