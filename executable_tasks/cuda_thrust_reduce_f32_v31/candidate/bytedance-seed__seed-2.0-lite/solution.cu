#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;

    for (int iter = 0; iter < iters; ++iter) {
        // Wrap raw device pointer for Thrust
        const thrust::device_ptr<const float> dev_input(input);
        // Execute optimized parallel reduction
        const float sum = thrust::reduce(dev_input, dev_input + n, 0.0f);
        // Update device output to match original memory access pattern
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }

    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}