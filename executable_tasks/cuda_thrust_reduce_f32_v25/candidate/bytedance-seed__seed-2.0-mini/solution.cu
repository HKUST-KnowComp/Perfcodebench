#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return 0.0f;
    }
    float* d_sum = nullptr;
    cudaMalloc(&d_sum, sizeof(float));
    const auto input_begin = thrust::device_pointer_cast(input);
    const auto input_end = input_begin + n;

    for (int iter = 0; iter < iters; ++iter) {
        thrust::reduce(
            thrust::device,
            input_begin,
            input_end,
            0.0f,
            thrust::plus<float>(),
            thrust::device_pointer_cast(d_sum)
        );
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_sum, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_sum);
    return actual;
}