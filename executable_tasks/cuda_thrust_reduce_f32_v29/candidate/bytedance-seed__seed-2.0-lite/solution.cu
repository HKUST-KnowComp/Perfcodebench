#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    float actual = 0.0f;
    const thrust::device_ptr<const float> dev_input = thrust::device_pointer_cast(input);
    
    for (int iter = 0; iter < iters; ++iter) {
        float sum = thrust::reduce(dev_input, dev_input + n, 0.0f);
        cudaMemcpyAsync(d_out, &sum, sizeof(float), cudaMemcpyHostToDevice);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}