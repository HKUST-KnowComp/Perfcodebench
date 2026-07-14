#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    
    // Wrap input device pointer for Thrust compatibility
    thrust::device_ptr<const float> dev_input(input);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Execute Thrust's highly optimized parallel reduction
        float sum = thrust::reduce(dev_input, dev_input + n, 0.0f);
        // Write result to device output to match baseline memory access pattern
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}