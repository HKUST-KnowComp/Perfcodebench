#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    
    // Wrap raw device pointer for Thrust compatibility
    auto d_input = thrust::device_pointer_cast(input);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Launch Thrust's optimized parallel reduction on the device
        float sum = thrust::reduce(d_input, d_input + n, 0.0f, thrust::plus<float>());
        // Match original kernel's behavior of writing the latest result to device output
        cudaMemcpy(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}