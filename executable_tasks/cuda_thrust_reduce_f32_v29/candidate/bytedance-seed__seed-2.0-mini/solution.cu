#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return 0.0f;
    }

    float* d_temp = nullptr;
    cudaMalloc(&d_temp, sizeof(float));
    thrust::device_ptr<float> d_temp_ptr(d_temp);
    const thrust::device_ptr<const float> dev_input(input);

    for (int iter = 0; iter < iters; ++iter) {
        thrust::reduce(
            dev_input,
            dev_input + n,
            0.0f,
            thrust::plus<float>(),
            d_temp_ptr
        );
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_temp, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_temp);

    return actual;
}